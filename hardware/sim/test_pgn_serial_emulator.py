import unittest
import os
import tempfile
import sys
from unittest.mock import patch, MagicMock
from pathlib import Path

# Add the parent directory to sys.path to import the module
sys.path.append(str(Path(__file__).parent))

from pgn_serial_emulator import list_pgn_files, load_pgn, get_available_ports

class TestPGNSerialEmulator(unittest.TestCase):
    def setUp(self):
        # Create a temporary directory for test PGN files
        self.temp_dir = tempfile.TemporaryDirectory()
        
        # Create sample PGN file
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
    
    @patch('serial.Serial')
    def test_mocked_serial_connection(self, mock_serial):
        """Test with a mocked serial connection"""
        # Setup mock serial port
        mock_instance = MagicMock()
        mock_serial.return_value = mock_instance
        
        # Setup context manager behavior for the mock
        mock_serial.return_value.__enter__.return_value = mock_instance
        
        # Simulate sending FEN strings
        positions = [
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
            "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
        ]
        
        headers = {
            "Event": "Test Game",
            "White": "Player 1",
            "Black": "Player 2"
        }
        
        # Import the function directly in the test method to avoid importing the whole module
        from pgn_serial_emulator import send_fen_game
        
        # Run the function for a short duration
        try:
            # Set a very short delay and run just once
            send_fen_game(
                port="/dev/mock",
                baud_rate=115200,
                delay=0.001,  # Use a very short delay
                positions=positions[:1],  # Use just the first position
                headers=headers,
                verbose=True
            )
        except KeyboardInterrupt:
            # The function might be interrupted by the test framework
            pass
        
        # Check that the serial port was opened with correct parameters
        mock_serial.assert_called_once_with("/dev/mock", 115200, timeout=1)
        
        # Check that data was written to the port
        self.assertTrue(mock_instance.write.called)
        
        # Make sure it tried to write a FEN position
        mock_instance.write.assert_any_call(positions[0].encode('utf-8'))
    
    @patch('serial.tools.list_ports.comports')
    def test_get_available_ports(self, mock_comports):
        # Create mock port objects
        mock_port1 = MagicMock()
        mock_port1.device = "/dev/ttyUSB0"
        mock_port1.description = "Mock Serial Port 1"
        
        mock_port2 = MagicMock()
        mock_port2.device = "/dev/ttyUSB1"
        mock_port2.description = "Mock Serial Port 2"
        
        # Set up mock to return our mock ports
        mock_comports.return_value = [mock_port1, mock_port2]
        
        # Test the function
        ports = get_available_ports()
        
        # Verify results
        self.assertEqual(len(ports), 2)
        self.assertEqual(ports[0].device, "/dev/ttyUSB0")
        self.assertEqual(ports[1].device, "/dev/ttyUSB1")

if __name__ == "__main__":
    unittest.main() 