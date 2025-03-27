import serial
import time
import argparse
import sys
import os
import glob
import chess
import chess.pgn
import io
from serial.tools import list_ports

def get_available_ports():
    """Get a list of available serial ports."""
    return list(list_ports.comports())

def select_port_interactively():
    """Allow user to select a port from available options."""
    ports = get_available_ports()
    if not ports:
        print("No serial ports detected on your system.")
        sys.exit(1)
    
    print("\nAvailable serial ports:")
    for i, port in enumerate(ports):
        print(f"  [{i}] {port.device} - {port.description}")
    
    try:
        choice = int(input("\nSelect port number: "))
        if 0 <= choice < len(ports):
            return ports[choice].device
        else:
            print(f"Invalid selection. Please choose a number between 0 and {len(ports)-1}")
            return select_port_interactively()
    except ValueError:
        print("Please enter a number.")
        return select_port_interactively()

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
    try:
        with open(pgn_file, 'r') as f:
            pgn_content = f.read()
            
        pgn = io.StringIO(pgn_content)
        game = chess.pgn.read_game(pgn)
        
        if not game:
            print(f"Could not parse PGN file: {pgn_file}")
            return []
            
        # Get game metadata
        headers = dict(game.headers)
        
        # Create a board and play through the moves to get FEN positions
        board = game.board()
        positions = [board.fen()]  # Start position
        
        for move in game.mainline_moves():
            board.push(move)
            positions.append(board.fen())
            
        return {"headers": headers, "positions": positions}
        
    except Exception as e:
        print(f"Error loading PGN file {pgn_file}: {e}")
        return {"headers": {}, "positions": []}

def parse_arguments():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        description="Send chess positions as FEN notation from PGN files over serial connection."
    )
    parser.add_argument(
        "--port", 
        default=None,
        help="Serial port (Windows: COMx, macOS: /dev/cu.*, Linux: /dev/ttyUSBx)"
    )
    parser.add_argument(
        "--baud", 
        type=int, 
        default=115200, 
        help="Baud rate"
    )
    parser.add_argument(
        "--delay", 
        type=float, 
        default=2, 
        help="Delay in seconds between sending positions"
    )
    parser.add_argument(
        "--list-ports",
        action="store_true",
        help="List available serial ports and select interactively"
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    parser.add_argument(
        "--simulate",
        action="store_true",
        help="Run in simulation mode without connecting to a port"
    )
    parser.add_argument(
        "--pgn-dir",
        default="hardware/sim/pgn",
        help="Directory containing PGN files"
    )
    parser.add_argument(
        "--list-games",
        action="store_true",
        help="List available PGN files and exit"
    )
    parser.add_argument(
        "--game",
        default=None,
        help="Specific game to play from the PGN directory"
    )
    parser.add_argument(
        "--loop",
        action="store_true",
        help="Continuously loop through the game positions"
    )
    return parser.parse_args()

def select_game_interactively(pgn_files):
    """Allow user to select a game from available PGN files."""
    if not pgn_files:
        print("No PGN files found.")
        sys.exit(1)
    
    print("\nAvailable PGN files:")
    games = list(pgn_files.items())
    for i, (game_id, file_path) in enumerate(games):
        game_info = load_pgn(file_path)
        print(f"  [{i}] {game_id}: {game_info['headers'].get('Event', 'Unknown')} - {game_info['headers'].get('White', 'Unknown')} vs {game_info['headers'].get('Black', 'Unknown')}")
    
    try:
        choice = int(input("\nSelect game number: "))
        if 0 <= choice < len(games):
            return games[choice][0]  # Return the game_id
        else:
            print(f"Invalid selection. Please choose a number between 0 and {len(games)-1}")
            return select_game_interactively(pgn_files)
    except ValueError:
        print("Please enter a number.")
        return select_game_interactively(pgn_files)

def send_fen_game(port, baud_rate, delay, positions, headers, verbose=False, loop=False):
    """
    Send FEN positions over serial connection.
    
    Args:
        port: Serial port name
        baud_rate: Baud rate for serial connection
        delay: Delay in seconds between positions
        positions: List of FEN positions to send
        headers: Game metadata
        verbose: Print verbose output
        loop: Whether to loop through positions continuously
    """
    try:
        with serial.Serial(port, baud_rate, timeout=1) as ser:
            print(f"Connected to {port} at {baud_rate} baud")
            print(f"Sending game: {headers.get('Event', 'Unknown')} - {headers.get('White', 'Unknown')} vs {headers.get('Black', 'Unknown')}")
            
            while True:  # Loop to support replaying the game
                for i, fen in enumerate(positions):
                    print(f"Move {i}: Sending FEN: {fen}")
                    ser.write(fen.encode('utf-8'))  # Send FEN as bytes
                    ser.write(b'\n')  # Newline to mark end of message
                    
                    if verbose:
                        print(f"Data sent. Waiting for {delay} seconds...")
                    
                    time.sleep(delay)  # Wait before sending next move
                    
                    if verbose:
                        try:
                            response = ser.read_all().decode('utf-8').strip()
                            if response:
                                print(f"Response received: {response}")
                        except Exception as e:
                            print(f"Error reading response: {e}")
                
                print("Game Over! All moves sent.")
                if not loop:
                    break
                print("Looping back to start...")
                
    except serial.SerialException as e:
        print(f"Error: Could not open serial port {port}: {e}")
        print("\nAvailable ports on your system:")
        ports = get_available_ports()
        if ports:
            for p in ports:
                print(f"  {p.device} - {p.description}")
        else:
            print("  No serial ports detected")
        
        # Offer to select a port interactively
        if ports:
            if input("\nWould you like to select a port and try again? (y/n): ").lower() == 'y':
                port = select_port_interactively()
                send_fen_game(port, baud_rate, delay, positions, headers, verbose, loop)
        sys.exit(1)

def simulate_fen_game(delay, positions, headers, verbose=False, loop=False):
    """
    Simulate sending FEN positions without a serial connection.
    
    Args:
        delay: Delay in seconds between positions
        positions: List of FEN positions to send
        headers: Game metadata
        verbose: Print verbose output
        loop: Whether to loop through positions continuously
    """
    print("SIMULATION MODE: Printing FEN positions without serial connection")
    print("This mode is useful when the port is already in use by another application")
    print(f"Simulating game: {headers.get('Event', 'Unknown')} - {headers.get('White', 'Unknown')} vs {headers.get('Black', 'Unknown')}")
    
    while True:  # Loop to support replaying the game
        for i, fen in enumerate(positions):
            player_turn = "White" if fen.split(' ')[1] == 'w' else "Black"
            print(f"Move {i}: {player_turn}'s turn")
            print(f"FEN: {fen}")
            if verbose:
                print(f"Waiting for {delay} seconds...")
            time.sleep(delay)
        
        print("Game Over! All moves simulated.")
        if not loop:
            break
        print("Looping back to start...")

if __name__ == "__main__":
    args = parse_arguments()
    
    # Check if PGN directory exists
    if not os.path.isdir(args.pgn_dir):
        print(f"PGN directory does not exist: {args.pgn_dir}")
        sys.exit(1)
    
    # Get available PGN files
    pgn_files = list_pgn_files(args.pgn_dir)
    if not pgn_files:
        print(f"No PGN files found in directory: {args.pgn_dir}")
        sys.exit(1)
    
    # List available games if requested
    if args.list_games:
        print("Available PGN files:")
        for game_id, file_path in pgn_files.items():
            game_info = load_pgn(file_path)
            print(f"  {game_id}: {game_info['headers'].get('Event', 'Unknown')} - {game_info['headers'].get('White', 'Unknown')} vs {game_info['headers'].get('Black', 'Unknown')}")
        sys.exit(0)
    
    # Select game
    game_id = args.game
    if game_id is None:
        # If no game specified, use an interactive selection
        game_id = select_game_interactively(pgn_files)
    elif game_id not in pgn_files:
        print(f"Game '{game_id}' not found. Available games: {', '.join(pgn_files.keys())}")
        game_id = select_game_interactively(pgn_files)
    
    # Load the game
    pgn_file = pgn_files[game_id]
    game_info = load_pgn(pgn_file)
    if not game_info["positions"]:
        print(f"Could not load positions from PGN file: {pgn_file}")
        sys.exit(1)
    
    # Use simulation mode if requested
    if args.simulate:
        simulate_fen_game(args.delay, game_info["positions"], game_info["headers"], args.verbose, args.loop)
        sys.exit(0)
    
    # Select port
    if args.list_ports:
        port = select_port_interactively()
    else:
        # If no port specified, use an interactive selection
        if args.port is None:
            # For macOS, try to select a reasonable default
            ports = get_available_ports()
            potential_ports = [p for p in ports if "cu." in p.device and "Bluetooth" not in p.device]
            if potential_ports:
                port = potential_ports[0].device
                print(f"Auto-selecting port: {port}")
            else:
                print("No port specified. Please select a port:")
                port = select_port_interactively()
        else:
            port = args.port
    
    print(f"PGN to Serial Chess Emulator - Ready to send {game_id} on {port}")
    print(f"Press Ctrl+C to abort at any time")
    
    try:
        send_fen_game(port, args.baud, args.delay, game_info["positions"], game_info["headers"], args.verbose, args.loop)
    except KeyboardInterrupt:
        print("\nEmulator stopped by user")
        sys.exit(0) 