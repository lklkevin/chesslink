import serial
import time
import argparse
import sys
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

def parse_arguments():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        description="Send chess positions as FEN notation over serial connection."
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
        default=10, 
        help="Delay in seconds between sending positions"
    )
    parser.add_argument(
        "--list",
        action="store_true",
        help="List available serial ports and select interactively"
    )
    return parser.parse_args()

# Immortal Game in FEN notation (move by move)
fen_immortal_game = [
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  # Start Position
    "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",  # 1. e4
    "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2",  # 1... e5
    "rnbqkbnr/pppp1ppp/8/4p3/2P1P3/8/PP1P1PPP/RNBQKBNR b KQkq c3 0 2",  # 2. f4
    "rnbqkbnr/pppp1p1p/8/4p3/2P1Pp2/8/PP1P2PP/RNBQKBNR w KQkq f3 0 3",  # 2... exf4
    "rnbqkbnr/pppp1p1p/8/4p3/2P1Pp2/5N2/PP1P2PP/RNBQKB1R b KQkq - 1 3",  # 3. Nf3
    "rnbqkb1r/pppp1p1p/8/4p3/2P1Pp2/5N2/PP1P2PP/RNBQK2R w KQkq - 2 4",  # 3... Qh4+
    "rnbqkb1r/pppp1p1p/8/4p3/2P1Pp2/3P1N2/PP3PPP/RNBQK2R b KQkq - 0 4",  # 4. d3
    "rnb1kb1r/pppp1p1p/8/4p3/2P1Pp2/3q1N2/PP3PPP/RNBQK2R w KQkq - 0 5",  # 4... Qf6
    "rnb1kb1r/pppp1p1p/5N2/4p3/2P1Pp2/3q4/PP3PPP/RNBQK2R b KQkq - 1 5",  # 5. Nc3
    "rnb1kb1r/pppp3p/5p2/4p3/2P1Pp2/3q4/PP3PPP/RNBQK2R w KQkq - 0 6",  # 5... g5
    "rnb1kb1r/pppp3p/5p2/4pP2/2P2p2/3q4/PP3PPP/RNBQK2R b KQkq - 0 6",  # 6. g3
    "rnb1kb1r/pppp3p/5p2/4pP2/2P5/3q2p1/PP3PP1/RNBQK2R w KQkq - 0 7",  # 6... g4
    "rnb1kb1r/pppp3p/5p2/4pP2/2P5/3q2p1/PP1N1PP1/R1BQK2R b KQkq - 1 7",  # 7. Nf3
    "rnb1kb1r/pppp3p/8/4pP2/2P5/3q2p1/PP1N1PP1/R1BQK2R w KQkq - 0 8",  # 7... Qxf3
    "rnb1kb1r/pppp3p/8/4pP2/2P5/3N2p1/PP3PP1/R1BQK2R b KQkq - 0 8",  # 8. Nd5
    "rnb1kb1r/pppp3p/8/4pP2/2P5/3N2p1/PP3PP1/R1BQ1K1R w kq - 1 9",  # 8... Kd8
    "rnb1kb1r/pppp3p/8/4pP2/2P5/3N2p1/PP3PP1/R1BQ1K1R b kq - 2 9",  # 9. h3
    "rnb1kb1r/pppp3p/8/4pP2/2P5/3N2p1/PP3P2/R1BQ1K1R w kq - 0 10",  # 9... g2+
    "rnb1kb1r/pppp3p/8/4pP2/2P5/3N2p1/PP3P1P/R1BQ1K2 b kq - 1 10",  # 10. Kf2
    "rnb1kb1r/pppp3p/8/4pP2/2P5/3N2p1/PP3PKP/R1BQ4 b kq - 2 11",  # 11. Kg1
    "rnb1kb1r/pppp3p/8/4pP2/2P5/3N2p1/PP3PKP/R1BQ4 w kq - 3 11",  # 11... g1=Q+
    "rnb1kb1r/pppp3p/8/4pP2/2P5/3N2p1/PP3PKP/R1BQ1q2 b kq - 4 12",  # 12. Qxg1
]

def send_fen_game(port, baud_rate, delay):
    """
    Send FEN positions over serial connection.
    
    Args:
        port: Serial port name
        baud_rate: Baud rate for serial connection
        delay: Delay in seconds between positions
    """
    try:
        with serial.Serial(port, baud_rate, timeout=1) as ser:
            print(f"Connected to {port} at {baud_rate} baud")
            for i, fen in enumerate(fen_immortal_game):
                print(f"Move {i + 1}: Sending FEN: {fen}")
                ser.write(fen.encode('utf-8'))  # Send FEN as bytes
                ser.write(b'\n')  # Newline to mark end of message
                time.sleep(delay)  # Wait before sending next move
            print("Game Over! All moves sent.")
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
                send_fen_game(port, baud_rate, delay)
        sys.exit(1)

if __name__ == "__main__":
    args = parse_arguments()
    
    if args.list:
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
    
    print(f"Chess FEN Emulator - Ready to send moves on {port}")
    print(f"Press Ctrl+C to abort at any time")
    
    try:
        send_fen_game(port, args.baud, args.delay)
    except KeyboardInterrupt:
        print("\nEmulator stopped by user")
        sys.exit(0)
