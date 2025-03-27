#!/bin/bash

# Script to start the PGN serial emulator
# Make sure to have activated the virtual environment first

# Activate virtual environment if not already activated
if [ -z "$VIRTUAL_ENV" ]; then
  source venv/bin/activate
fi

# Default to simulation mode if no --port is specified
if [[ "$*" != *"--port"* ]]; then
  SIMULATE="--simulate"
else
  SIMULATE=""
fi

# Start the PGN serial emulator
python hardware/sim/pgn_serial_emulator.py $SIMULATE --pgn-dir hardware/sim/pgn "$@"

# Exit message when the emulator is stopped
echo "PGN serial emulator stopped" 