import serial
import threading
import time

# Map regions to their expected board positions
REGION_MAP = {
    'R1': ['a1', 'b1', 'a2', 'b2'],
    'R2': ['c1', 'd1', 'c2', 'd2'],
    'R3': ['e1', 'f1', 'e2', 'f2'],
    'R4': ['g1', 'h1', 'g2', 'h2'],
}

# Global state
board_state = {}

def read_from_port(name, port):
    ser = serial.Serial(port, 9600)
    while True:
        line = ser.readline().decode('utf-8').strip()
        print(f"[{name}] {line}")
        # Expected format: R1:a1P b1. a2. b2.
        try:
            region, data = line.split(":")
            squares = data.strip().split()
            for s in squares:
                sq, val = s[:2], s[2:]
                board_state[sq] = val
        except Exception as e:
            print(f"Error parsing from {name}: {e}")

# Start threads for each Nano
ports = {
    'R1': '/dev/cu.usbserial-120',
    'R2': '/dev/cu.usbserial-130',
    # 'R3': '/dev/cu.usbserial-1412',
    # 'R4': '/dev/cu.usbserial-1413',
}

for name, port in ports.items():
    t = threading.Thread(target=read_from_port, args=(name, port))
    t.daemon = True
    t.start()

# Periodically print full FEN string
def fen_from_board():
    full_rows = []
    for rank in range(8, 0, -1):
        row = ''
        empty = 0
        for file in 'abcdefgh':
            sq = f"{file}{rank}"
            piece = board_state.get(sq, '.')
            if piece == '.':
                empty += 1
            else:
                if empty > 0:
                    row += str(empty)
                    empty = 0
                row += piece
        if empty > 0:
            row += str(empty)
        full_rows.append(row)
    return "/".join(full_rows)

while True:
    time.sleep(1)
    print("Current FEN: ", fen_from_board())
