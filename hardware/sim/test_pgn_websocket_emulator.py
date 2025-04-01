import unittest
import asyncio
import websockets
import json
import os
import tempfile
import sys
from pathlib import Path

# Add the parent directory to sys.path to import the module
sys.path.append(str(Path(__file__).parent))

from pgn_websocket_emulator import list_pgn_files, load_pgn

class TestPGNWebSocketEmulator(unittest.TestCase):
    def setUp(self):
        # Create a temporary directory for test PGN files
        self.temp_dir = tempfile.TemporaryDirectory()
        
        # Create sample PGN files
        self.sample_pgn_content = """
[Event "Test Game"]
[Site "Test Site"]
[Date "2023.01.01"]
[Round "1"]
[White "Player 1"]
[Black "Player 2"]
[Result "1-0"]

1. e4 e5 2. Nf3 Nc6 3. Bc4 Bc5 4. O-O Nf6 5. d4 Bxd4 6. Nxd4 Nxd4 7. f4 d6 8. fxe5 dxe5 9. Bg5 O-O
        """
        
        self.pgn_path = os.path.join(self.temp_dir.name, "test_game.pgn")
        with open(self.pgn_path, "w", encoding="utf-8") as f:
            f.write(self.sample_pgn_content)
    
    def tearDown(self):
        # Clean up the temporary directory
        self.temp_dir.cleanup()
    
    def test_list_pgn_files(self):
        # Test that list_pgn_files correctly identifies PGN files
        files = list_pgn_files(self.temp_dir.name)
        self.assertEqual(len(files), 1)
        self.assertIn("test_game", files)
        self.assertEqual(files["test_game"], self.pgn_path)
    
    def test_load_pgn(self):
        # Test that load_pgn correctly loads and parses a PGN file
        game_info = load_pgn(self.pgn_path)
        
        # Check headers
        self.assertEqual(game_info["headers"]["Event"], "Test Game")
        self.assertEqual(game_info["headers"]["White"], "Player 1")
        self.assertEqual(game_info["headers"]["Black"], "Player 2")
        
        # Check positions
        self.assertGreater(len(game_info["positions"]), 1)
        # First position should be the standard starting position
        self.assertTrue(game_info["positions"][0].startswith("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"))
        
        # Last position should be different from the first
        self.assertNotEqual(game_info["positions"][0], game_info["positions"][-1])

# WebSocket client for testing the server
async def ws_client_test(uri):
    async with websockets.connect(uri) as websocket:
        # Receive the info message
        response = await websocket.recv()
        data = json.loads(response)
        return data

class TestWebSocketServer(unittest.IsolatedAsyncioTestCase):
    async def test_server_connection(self):
        """Test that we can connect to a local WebSocket server.
        
        Note: This test requires starting the server in a separate process before running.
        Use: `python pgn_websocket_emulator.py --port 8765 --delay 0.1` in a separate terminal.
        """
        try:
            # Try to connect to a running server
            data = await asyncio.wait_for(ws_client_test("ws://localhost:8765"), timeout=2)
            
            # Check that we received a valid info message
            self.assertEqual(data["type"], "info")
            self.assertIn("total_positions", data)
            self.assertIn("available_games", data)
            print("Successfully connected to WebSocket server and received data")
        except (ConnectionRefusedError, asyncio.TimeoutError):
            self.skipTest("WebSocket server not running. Start it with: python pgn_websocket_emulator.py --port 8765 --delay 0.1")

if __name__ == "__main__":
    unittest.main() 