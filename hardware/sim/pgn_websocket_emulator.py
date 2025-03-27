import asyncio
import websockets
import logging
import json
import argparse
import time
import os
import glob
import chess
import chess.pgn
import io

# Enable logging to see connection issues
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger('chess-pgn-websocket')

# Make args global so handler can access them
args = None

# Cache for PGN games to avoid reloading from disk
pgn_cache = {}

def list_pgn_files(pgn_dir):
    """List all PGN files in the given directory."""
    pgn_files = glob.glob(os.path.join(pgn_dir, "*.pgn"))
    result = {}
    
    for pgn_file in pgn_files:
        game_id = os.path.basename(pgn_file).replace('.pgn', '')
        result[game_id] = pgn_file
        
    return result

def load_pgn(pgn_file):
    """Load a PGN file and return a list of FEN positions from the game."""
    if pgn_file in pgn_cache:
        return pgn_cache[pgn_file]
        
    try:
        with open(pgn_file, 'r') as f:
            pgn_content = f.read()
            
        pgn = io.StringIO(pgn_content)
        game = chess.pgn.read_game(pgn)
        
        if not game:
            logger.error(f"Could not parse PGN file: {pgn_file}")
            return []
            
        # Get game metadata
        headers = dict(game.headers)
        
        # Create a board and play through the moves to get FEN positions
        board = game.board()
        positions = [board.fen()]  # Start position
        
        game_info = {
            "headers": headers,
            "positions": positions
        }
        
        for move in game.mainline_moves():
            board.push(move)
            positions.append(board.fen())
            
        # Cache the result
        pgn_cache[pgn_file] = game_info
        return game_info
        
    except Exception as e:
        logger.error(f"Error loading PGN file {pgn_file}: {e}")
        return {"headers": {}, "positions": []}

async def handle_client(websocket):
    """Handle a client connection to the WebSocket server."""
    try:
        # Get connection info if available
        client_ip = websocket.remote_address[0] if hasattr(websocket, 'remote_address') else "unknown"
        
        # Default to first PGN if available
        pgn_files = list_pgn_files(args.pgn_dir)
        if not pgn_files:
            await send_message_safely(websocket, {
                "type": "error",
                "message": "No PGN files found in directory"
            })
            return
            
        game_id = list(pgn_files.keys())[0]
        
        # Try to get game from request path if available
        if hasattr(websocket, 'path'):
            # Parse query parameters to get the game ID
            from urllib.parse import parse_qs, urlparse
            query_params = parse_qs(urlparse(websocket.path).query)
            requested_game = query_params.get('game', [None])[0]
            
            if requested_game and requested_game in pgn_files:
                game_id = requested_game
        
        # Load the requested PGN file
        pgn_file = pgn_files[game_id]
        game_info = load_pgn(pgn_file)
        
        if not game_info["positions"]:
            await send_message_safely(websocket, {
                "type": "error",
                "message": f"Could not load positions from PGN file: {pgn_file}"
            })
            return
        
        # Get the list of available games to send to the client
        available_games = []
        for game_id, file_path in pgn_files.items():
            game_data = load_pgn(file_path)
            available_games.append({
                "id": game_id,
                "name": game_data["headers"].get("Event", game_id),
                "white": game_data["headers"].get("White", "Unknown"),
                "black": game_data["headers"].get("Black", "Unknown"),
                "result": game_data["headers"].get("Result", "*"),
                "moves_count": len(game_data["positions"]) - 1
            })
        
        logger.info(f"Client connected from {client_ip} - Streaming game: {game_id}")
        
        # Send initial information about the game and available PGNs
        info_message = {
            "type": "info",
            "message": f"Starting {game_id} game simulation",
            "total_positions": len(game_info["positions"]),
            "headers": game_info["headers"],
            "available_games": available_games
        }
        
        await send_message_safely(websocket, info_message)
        
        # Send each position with a delay
        move_number = 0
        connection_open = True
        
        try:
            for fen in game_info["positions"]:
                # Check if connection is still open before sending
                if not connection_open:
                    logger.info("Connection closed, stopping position updates")
                    break
                    
                # Calculate whose move it is from the FEN
                parts = fen.split()
                active_player = "w" if parts[1] == "w" else "b"
                
                message = {
                    "type": "position",
                    "fen": fen,
                    "move": move_number,
                    "active_player": active_player
                }
                
                if not await send_message_safely(websocket, message):
                    connection_open = False
                    break
                    
                move_number += 1
                try:
                    await asyncio.sleep(args.delay)
                except asyncio.CancelledError:
                    logger.info("Sleep interrupted, stopping position updates")
                    break
                
            # After sending all positions, loop back if continuous playback is enabled
            if args.loop and connection_open:
                logger.info(f"Completed game cycle, looping back to start")
                while connection_open:
                    move_number = 0
                    for fen in game_info["positions"]:
                        # Check if connection is still open before sending
                        if not connection_open:
                            logger.info("Connection closed during loop, stopping position updates")
                            break
                        
                        # Calculate whose move it is from the FEN
                        parts = fen.split()
                        active_player = "w" if parts[1] == "w" else "b"
                        
                        message = {
                            "type": "position",
                            "fen": fen,
                            "move": move_number,
                            "active_player": active_player
                        }
                        
                        if not await send_message_safely(websocket, message):
                            connection_open = False
                            break
                            
                        move_number += 1
                        try:
                            await asyncio.sleep(args.delay)
                        except asyncio.CancelledError:
                            logger.info("Sleep interrupted during loop, stopping position updates")
                            break
            else:
                # Send final message indicating completion
                if connection_open:
                    complete_message = {
                        "type": "info",
                        "message": f"Game simulation complete"
                    }
                    await send_message_safely(websocket, complete_message)
        except asyncio.CancelledError:
            logger.info("Handler task cancelled, stopping position updates")
            
    except websockets.exceptions.ConnectionClosed:
        logger.info(f"Client disconnected from {client_ip}")
    except Exception as e:
        logger.error(f"Error in handler: {e}")
        logger.exception("Detailed exception information:")

# Helper function to safely send messages
async def send_message_safely(websocket, message):
    """Send a message to the WebSocket client, handling errors gracefully."""
    try:
        # Convert message to JSON before sending
        json_message = json.dumps(message)
        await websocket.send(json_message)
        
        # Only log position updates at debug level to reduce noise
        if message.get("type") == "position":
            logger.debug(f"Sent position {message.get('move')+1}: {message.get('fen')}")
        else:
            logger.info(f"Sent message: {message.get('type')}")
        return True
    except websockets.exceptions.ConnectionClosed:
        logger.info("Connection closed while sending message")
        return False
    except Exception as e:
        logger.error(f"Error sending message: {e}")
        return False

def parse_args():
    parser = argparse.ArgumentParser(description="Chess PGN WebSocket Emulator")
    parser.add_argument("--port", type=int, default=8765, help="Port to run the server on")
    parser.add_argument("--delay", type=float, default=3.0, help="Delay between positions in seconds")
    parser.add_argument("--loop", action="store_true", help="Continuously loop through positions")
    parser.add_argument("--pgn-dir", default="hardware/sim/pgn", help="Directory containing PGN files")
    parser.add_argument("--list", action="store_true", help="List available PGN files and exit")
    return parser.parse_args()

async def main():
    """Main entry point for the WebSocket server."""
    global args
    args = parse_args()
    
    # Check if PGN directory exists
    if not os.path.isdir(args.pgn_dir):
        logger.error(f"PGN directory does not exist: {args.pgn_dir}")
        return
    
    # List PGN files if requested
    pgn_files = list_pgn_files(args.pgn_dir)
    if not pgn_files:
        logger.error(f"No PGN files found in directory: {args.pgn_dir}")
        return
        
    if args.list:
        print("Available PGN files:")
        for game_id, file_path in pgn_files.items():
            game_info = load_pgn(file_path)
            print(f"  - {game_id}: {game_info['headers'].get('Event', 'Unknown')} ({game_info['headers'].get('White', 'Unknown')} vs {game_info['headers'].get('Black', 'Unknown')})")
        return
    
    # Start the WebSocket server
    try:
        server = await websockets.serve(
            handle_client, 
            "0.0.0.0", 
            args.port,
            # Allow connections from any origin
            origins=None,
            # Set ping interval to keep connections alive
            ping_interval=20,
            ping_timeout=10
        )
        
        logger.info(f"Starting PGN WebSocket server at ws://localhost:{args.port}")
        logger.info(f"Position delay: {args.delay} seconds")
        logger.info(f"Looping: {'enabled' if args.loop else 'disabled'}")
        logger.info(f"PGN directory: {args.pgn_dir}")
        logger.info(f"Available games: {', '.join(pgn_files.keys())}")
        
        # Keep the server running until interrupted
        try:
            await asyncio.Future()
        except asyncio.CancelledError:
            logger.info("Server task cancelled")
        finally:
            server.close()
            await server.wait_closed()
            logger.info("Server closed gracefully")
    except OSError as e:
        logger.error(f"Failed to start server: {e}")
        if "Address already in use" in str(e):
            logger.error(f"Port {args.port} is already in use. Try a different port with --port option.")
        return 1

if __name__ == "__main__":
    try:
        # Use asyncio.run() to properly manage the event loop
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.info("Server stopped by user")
        print("\nWebSocket server stopped by user. Goodbye!")
    except Exception as e:
        logger.error(f"Server error: {e}")
        logger.exception("Detailed exception information:") 