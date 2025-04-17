import serial
import threading
import json
import time

board_state = {}

def is_json(line: str) -> bool:
    return line.startswith("{") and line.endswith("}")

def read_from_port(region, port):
    ser = serial.Serial(port, 9600)
    while True:
        try:
            line = ser.readline().decode('utf-8').strip()

            if not is_json(line):
                # print(f"[{region}][DEBUG] {line}")
                continue

            data = json.loads(line)
            if "squares" in data:
                for square, piece in data["squares"].items():
                    board_state[square] = piece
        except Exception as e:
            print(f"[{region}][ERROR] Failed to parse: {e}")

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

# Map regions to ports
ports = {
    'R1': '/dev/cu.usbserial-130',
    'R2': '/dev/cu.usbserial-120',
    # 'R3': '/dev/cu.usbserial-1412',
    # 'R4': '/dev/cu.usbserial-1413',
}

# Start a thread for each serial device
for region, port in ports.items():
    t = threading.Thread(target=read_from_port, args=(region, port))
    t.daemon = True
    t.start()

# Print updated FEN every second
while True:
    time.sleep(1)
    print("Current FEN:", fen_from_board())

