import serial
import time
import sys

# Simplified test FEN strings
TEST_POSITIONS = [
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  # Start Position
    "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",  # 1. e4
    "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2",  # 1... e5
    "rnbqkbnr/pppp1ppp/8/4p3/2P1P3/8/PP1P1PPP/RNBQKBNR b KQkq c3 0 2"  # 2. c4
]

# Serial port to use - CHANGE THIS to match your system
PORT = "/dev/cu.debug-console"  
BAUD_RATE = 115200
DELAY = 3  # Seconds between positions

print(f"Simple Serial Test - Will send 4 FEN positions to {PORT}")
print("Press Ctrl+C to abort at any time")

try:
    # Open the serial port
    with serial.Serial(PORT, BAUD_RATE, timeout=1) as ser:
        print(f"Connected to {PORT} at {BAUD_RATE} baud")
        
        # Send each position with a delay
        for i, fen in enumerate(TEST_POSITIONS):
            print(f"Position {i+1}/{len(TEST_POSITIONS)}: Sending: {fen}")
            
            # Send the FEN string followed by a newline
            ser.write(fen.encode('utf-8'))
            ser.write(b'\n')
            
            # Wait before sending the next position
            print(f"Waiting {DELAY} seconds...")
            time.sleep(DELAY)
            
        print("Test completed! All positions sent.")
        
except serial.SerialException as e:
    print(f"Error: Could not open serial port {PORT}: {e}")
    sys.exit(1)
except KeyboardInterrupt:
    print("\nTest stopped by user")
    sys.exit(0) 