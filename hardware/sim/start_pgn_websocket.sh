#!/bin/bash

# Script to start the PGN WebSocket server
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
    echo "  pip install chess websockets"
    exit 1
  fi
fi

# Check for required modules
if ! python -c "import chess, websockets" &> /dev/null; then
  echo "Error: Required modules not found. Installing them now..."
  pip install chess websockets
  echo "Modules installed successfully."
fi

# Default to looping mode unless --no-loop is specified
if [[ "$*" == *"--no-loop"* ]]; then
  LOOP=""
else
  LOOP="--loop"
fi

# Set default port if not specified
if [[ "$*" != *"--port"* ]]; then
  PORT="--port 8765"
else
  PORT=""
fi

# Ensure PYTHONPATH includes project directory
export PYTHONPATH="$PROJECT_DIR:$PYTHONPATH"

echo "Starting PGN WebSocket server..."
echo "Connect to ws://localhost:8765 from your web app"
echo "Press Ctrl+C to stop the server"

# Start the PGN WebSocket server
python "$SCRIPT_DIR/pgn_websocket_emulator.py" $LOOP $PORT --pgn-dir "$SCRIPT_DIR/pgn" "$@"

# Exit message when the server is stopped
echo "PGN WebSocket server stopped" 