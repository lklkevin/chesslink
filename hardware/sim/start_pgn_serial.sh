#!/bin/bash

# Script to start the PGN serial emulator
# Make sure to have activated the virtual environment first

# Exit on error
set -e

# Get script directory for relative paths
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_DIR="$(dirname "$(dirname "$SCRIPT_DIR")")"

# Activate virtual environment if not already activated
if [ -z "$VIRTUAL_ENV" ]; then
  if [ -f "$PROJECT_DIR/venv/bin/activate" ]; then
    source "$PROJECT_DIR/venv/bin/activate"
    echo "Virtual environment activated."
  else
    echo "Error: Virtual environment not found at $PROJECT_DIR/venv"
    echo "Please create a virtual environment and install required packages:"
    echo "  python -m venv venv"
    echo "  source venv/bin/activate"
    echo "  pip install chess"
    exit 1
  fi
fi

# Check for chess module
if ! python -c "import chess" &> /dev/null; then
  echo "Error: 'chess' module not found. Installing it now..."
  pip install chess
  echo "Chess module installed successfully."
fi

# Default to simulation mode if no --port is specified
if [[ "$*" != *"--port"* ]]; then
  SIMULATE="--simulate"
else
  SIMULATE=""
fi

# Ensure PYTHONPATH includes project directory
export PYTHONPATH="$PROJECT_DIR:$PYTHONPATH"

echo "Starting PGN serial emulator..."
# Start the PGN serial emulator
python "$SCRIPT_DIR/pgn_serial_emulator.py" $SIMULATE --pgn-dir "$SCRIPT_DIR/pgn" "$@"

# Exit message when the emulator is stopped
echo "PGN serial emulator stopped" 