import asyncio
import websockets
import logging
import json
import argparse
import time
from urllib.parse import parse_qs, urlparse

# Enable logging to see connection issues
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger('chess-websocket')

# Dictionary of famous chess games and their FEN positions
CHESS_GAMES = {
    "immortal": [  # Anderssen's Immortal Game
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  # Start Position
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",  # 1. e4
        "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2",  # 1... e5
        "rnbqkbnr/pppp1ppp/8/4p3/2P1P3/8/PP1P1PPP/RNBQKBNR b KQkq c3 0 2",  # 2. c4
        "rnbqkbnr/pppp1p1p/8/4p3/2P1Pp2/8/PP1P2PP/RNBQKBNR w KQkq f3 0 3",  # 2... f5
        "rnbqkbnr/pppp1p1p/8/4p3/2P1Pp2/5N2/PP1P2PP/RNBQKB1R b KQkq - 1 3",  # 3. Nf3
        "rnbqkb1r/pppp1p1p/8/4p3/2P1Pp2/5N2/PP1P2PP/RNBQKB1R w KQkq - 2 4",  # 3... Nh6
        "rnbqkb1r/pppp1p1p/8/4p3/2P1Pp2/3P1N2/PP4PP/RNBQKB1R b KQkq - 0 4",  # 4. d3
        "rnb1kb1r/pppp1p1p/8/4p3/2P1Pp2/3P1N2/PP4PP/RNBQKB1R w KQkq - 1 5",  # 4... Qh4+
        "rnb1kb1r/pppp1p1p/8/4p3/2P1Pp2/3P1N2/PP3QPP/RNBQKB1R b KQkq - 2 5",  # 5. g3
    ],
    "brilliancy": [  # Kasparov's Immortal - Kasparov vs Topalov (1999)
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  # Start Position
        "rnbqkbnr/pppppppp/8/8/3P4/8/PPP1PPPP/RNBQKBNR b KQkq d3 0 1",  # 1. d4
        "rnbqkbnr/ppp1pppp/8/3p4/3P4/8/PPP1PPPP/RNBQKBNR w KQkq d6 0 2",  # 1... d5
        "rnbqkbnr/ppp1pppp/8/3p4/3P4/2N5/PPP1PPPP/R1BQKBNR b KQkq - 1 2",  # 2. Nc3
        "rn1qkbnr/ppp1pppp/8/3p4/3P2b1/2N5/PPP1PPPP/R1BQKBNR w KQkq - 2 3",  # 2... Bg4
        "rn1qkbnr/ppp1pppp/8/3p4/3P2b1/2N2N2/PPP1PPPP/R1BQKB1R b KQkq - 3 3",  # 3. Nf3
        "rn1qkbnr/ppp1pp1p/6p1/3p4/3P2b1/2N2N2/PPP1PPPP/R1BQKB1R w KQkq - 0 4",  # 3... g6
        "rn1qkbnr/ppp1pp1p/6p1/3p4/3P2b1/2N2N2/PPP1PPPP/R1BQKB1R w KQkq - 0 4",  # 3... g6
        "rn1qkbnr/ppp1pp1p/6p1/3p4/3P2b1/2N2N2/PPPBPPPP/R2QKB1R b KQkq - 1 4",  # 4. Bd2
        "rn2kbnr/ppp1pp1p/5qp1/3p4/3P2b1/2N2N2/PPPBPPPP/R2QKB1R w KQkq - 2 5",  # 4... Qd6
    ],
    "opera": [  # Opera Game - Morphy vs Duke of Brunswick (1858)
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  # Start Position
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",  # 1. e4
        "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2",  # 1... e5
        "rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2",  # 2. Nf3
        "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3",  # 2... Nc6
        "r1bqkbnr/pppp1ppp/2n5/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 3 3",  # 3. Bc4
        "r1bqkbnr/ppp2ppp/2n5/3pp3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq d6 0 4",  # 3... d5
        "r1bqkbnr/ppp2ppp/2n5/3Pp3/2B5/5N2/PPPP1PPP/RNBQK2R b KQkq - 0 4",  # 4. exd5
        "r1bqkbnr/ppp2ppp/8/3pn3/2B5/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 5",  # 4... Nxd5
        "r1bqkbnr/ppp2ppp/8/3pn3/2B5/5N2/PPPP1PPP/RNBQ1RK1 b kq - 1 5",  # 5. O-O
    ],
    "miniatures": [  # Short, decisive games
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  # Start Position
        "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",  # 1. e4
        "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2",  # 1... e5
        "rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2",  # 2. Nf3
        "rnbqk1nr/pppp1ppp/8/2b1p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3",  # 2... Bc5
        "rnbqk1nr/pppp1ppp/8/2b1p3/1P2P3/5N2/P1PP1PPP/RNBQKB1R b KQkq b3 0 3",  # 3. b4
        "rnbqk1nr/pppp1ppp/8/4p3/1b2P3/5N2/P1PP1PPP/RNBQKB1R w KQkq - 0 4",  # 3... Bxb4
        "rnbqk1nr/pppp1ppp/8/4p3/1b2P3/1P3N2/P1PP1PPP/RNBQKB1R b KQkq - 0 4",  # 4. c3
        "rnbqk1nr/pppp1ppp/8/8/4p3/1P3N2/P1PP1PPP/RNBQKB1R w KQkq - 0 5",  # 4... Ba5
        "rnbqk1nr/pppp1ppp/8/8/4N3/1P6/P1PP1PPP/RNBQKB1R b KQkq - 0 5",  # 5. Nxe5
    ]
}

# Make args global so handler can access them
args = None

# Update the handler function to better handle connection states
async def handle_client(websocket):
    """Handle a client connection to the WebSocket server."""
    try:
        # Get connection info if available
        client_ip = websocket.remote_address[0] if hasattr(websocket, 'remote_address') else "unknown"
        
        # Use the default game since we can't access path
        game_type = "immortal"
        
        # Try to get game from request path if available
        if hasattr(websocket, 'path'):
            # Parse query parameters to get the game type
            query_params = parse_qs(urlparse(websocket.path).query)
            game_type = query_params.get('game', ['immortal'])[0]
        
        # Validate game type
        if game_type not in CHESS_GAMES:
            logger.warning(f"Invalid game type: {game_type}, defaulting to 'immortal'")
            game_type = 'immortal'
        
        # Get the positions for the selected game
        positions = CHESS_GAMES[game_type]
        
        logger.info(f"Client connected from {client_ip} - Streaming game: {game_type}")
        
        # Send initial information about the game
        info_message = {
            "type": "info",
            "message": f"Starting {game_type} game simulation",
            "total_positions": len(positions)
        }
        
        await send_message_safely(websocket, info_message)
        
        # Send each position with a delay
        move_number = 0
        connection_open = True
        for fen in positions:
            # Check if connection is still open before sending
            if not connection_open:
                logger.info("Connection closed, stopping position updates")
                break
                
            message = {
                "type": "position",
                "fen": fen,
                "move": move_number
            }
            
            if not await send_message_safely(websocket, message):
                connection_open = False
                break
                
            move_number += 1
            await asyncio.sleep(args.delay)
            
        # After sending all positions, loop back if continuous playback is enabled
        if args.loop and connection_open:
            logger.info(f"Completed game cycle, looping back to start")
            while connection_open:
                move_number = 0
                for fen in positions:
                    # Check if connection is still open before sending
                    if not connection_open:
                        logger.info("Connection closed during loop, stopping position updates")
                        break
                        
                    message = {
                        "type": "position",
                        "fen": fen,
                        "move": move_number
                    }
                    
                    if not await send_message_safely(websocket, message):
                        connection_open = False
                        break
                        
                    move_number += 1
                    await asyncio.sleep(args.delay)
        else:
            # Send final message indicating completion
            if connection_open:
                complete_message = {
                    "type": "info",
                    "message": f"Game simulation complete"
                }
                await send_message_safely(websocket, complete_message)
            
    except websockets.exceptions.ConnectionClosed:
        logger.info(f"Client disconnected from {client_ip}")
    except Exception as e:
        logger.error(f"Error in handler: {e}")

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
    parser = argparse.ArgumentParser(description="Chess WebSocket Emulator")
    parser.add_argument("--port", type=int, default=8765, help="Port to run the server on")
    parser.add_argument("--delay", type=float, default=3.0, help="Delay between positions in seconds")
    parser.add_argument("--loop", action="store_true", help="Continuously loop through positions")
    parser.add_argument("--list", action="store_true", help="List available games and exit")
    return parser.parse_args()

async def main():
    """Main entry point for the WebSocket server."""
    global args
    args = parse_args()
    
    # List games if requested
    if args.list:
        print("Available games:")
        for game, positions in CHESS_GAMES.items():
            print(f"  - {game}: {len(positions)} positions")
        return
    
    # Start the WebSocket server
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
    
    logger.info(f"Starting WebSocket server at ws://localhost:{args.port}")
    logger.info(f"Position delay: {args.delay} seconds")
    logger.info(f"Looping: {'enabled' if args.loop else 'disabled'}")
    
    # Keep the server running until interrupted
    await asyncio.Future()

if __name__ == "__main__":
    try:
        # Use asyncio.run() to properly manage the event loop
        asyncio.run(main())
    except KeyboardInterrupt:
        logger.info("Server stopped by user")
    except Exception as e:
        logger.error(f"Server error: {e}") 