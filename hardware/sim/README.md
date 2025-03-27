# Chess FEN Emulator

This script emulates a chess game by sending FEN (Forsyth-Edwards Notation) positions over a serial connection.

## Installation Instructions

### Step 1: Install Python
Make sure Python 3.6 or newer is installed on your system.
- Download from [python.org](https://www.python.org/downloads/)
- Verify installation with `python --version` or `python3 --version`

### Step 2: Install Required Dependencies
Install the pyserial package using pip:

```bash
# For Windows/Linux
pip install pyserial

# For macOS (if using pip3)
pip3 install pyserial
```

## Running the Emulator

### Step 1: Connect Your Device
Connect your Arduino or other serial device to your computer.

### Step 2: Identify the Serial Port
- **Windows**: Look for "COMx" (e.g., COM3, COM4) in Device Manager
- **macOS**: Look for "/dev/cu.*" ports in Terminal with `ls /dev/cu.*` (like `/dev/cu.A90Pro` in your case)
- **Linux**: Look for "/dev/ttyUSBx" or "/dev/ttyACMx" in Terminal with `ls /dev/tty*`

### Step 3: Run the Script
Run the script with your serial port:

```bash
# Specify the port
python emulator.py --port COM3 --baud 115200

# For macOS (use the exact port name from your system)
python emulator.py --port /dev/cu.debug-console --baud 115200

# To see available ports and select interactively
python emulator.py --list

# Or use default values
python emulator.py
```

### Common Issues

1. **Permission Denied**: You may need to run with admin/sudo privileges or add yourself to the dialout group on Linux
2. **Port Not Found**: Double check your port name and connection
3. **Serial Device Busy**: Make sure no other program is using the serial port

## Configuration

Edit `emulator.py` to:
- Change the default serial port and baud rate
- Modify the delay between moves
- Add different games or positions

## PGN Serial Emulator

The `pgn_serial_emulator.py` script allows you to send chess positions from PGN files as FEN strings over a serial connection. This is useful for testing the ChessLink hardware or software without using the real hardware.

### Usage

```bash
# List available PGN files
./start_pgn_serial.sh --list-games

# Run in simulation mode with a specific game (default)
./start_pgn_serial.sh --game immortal

# Connect to a hardware device
./start_pgn_serial.sh --port /dev/cu.usbserial-1410 --game kasparov_topalov

# List available serial ports
./start_pgn_serial.sh --list-ports

# Enable verbose mode
./start_pgn_serial.sh --verbose

# Loop through the game positions
./start_pgn_serial.sh --loop
```

### Options

- `--port`: Serial port to connect to
- `--baud`: Baud rate (default: 115200)
- `--delay`: Delay in seconds between positions (default: 2)
- `--list-ports`: List available serial ports and select interactively
- `--verbose`: Enable verbose output
- `--simulate`: Run in simulation mode without connecting to a port (default if no port specified)
- `--pgn-dir`: Directory containing PGN files (default: hardware/sim/pgn)
- `--list-games`: List available PGN files and exit
- `--game`: Specific game to play from the PGN directory
- `--loop`: Continuously loop through the game positions
