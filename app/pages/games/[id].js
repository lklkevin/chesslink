import { useState, useEffect } from 'react';
import { useRouter } from 'next/router';
import Link from 'next/link';
import { Chessboard } from 'react-chessboard';
import { Chess } from 'chess.js';

export default function GameDetail() {
  const router = useRouter();
  const { id } = router.query;
  
  const [game, setGame] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  const [ports, setPorts] = useState([]);
  const [selectedPort, setSelectedPort] = useState('');
  const [connected, setConnected] = useState(false);
  const [pollingInterval, setPollingInterval] = useState(null);
  
  // Chess board state
  const [selectedMoveIndex, setSelectedMoveIndex] = useState(0);
  const [chessInstance, setChessInstance] = useState(null);
  const [boardOrientation, setBoardOrientation] = useState('white');
  const [gameStatus, setGameStatus] = useState({ 
    isOver: false, 
    result: null, 
    reason: null 
  });

  // Fetch game data on initial load
  useEffect(() => {
    if (!id) return;
    fetchGameData();
    fetchSerialPorts();
  }, [id]);

  // Initialize chess instance when game data is loaded or updated
  useEffect(() => {
    if (game && game.moves && game.moves.length > 0) {
      // Default to showing the latest position
      updateBoardPosition(game.moves.length - 1);
    }
  }, [game]);

  // Set up polling when connected
  useEffect(() => {
    // Clear existing interval when component unmounts or connection status changes
    return () => {
      if (pollingInterval) {
        clearInterval(pollingInterval);
      }
    };
  }, [pollingInterval]);

  // Start polling when connected
  useEffect(() => {
    if (connected) {
      // Poll every 2 seconds
      const interval = setInterval(() => {
        fetchActiveGameState();
      }, 2000);
      setPollingInterval(interval);
    } else {
      // Clear interval when disconnected
      if (pollingInterval) {
        clearInterval(pollingInterval);
        setPollingInterval(null);
      }
    }
  }, [connected, id]);

  const updateBoardPosition = (moveIndex) => {
    if (!game || !game.moves || moveIndex < 0 || moveIndex >= game.moves.length) return;
    
    try {
      const fen = game.moves[moveIndex].fen;
      const chess = new Chess(fen);
      setChessInstance(chess);
      setSelectedMoveIndex(moveIndex);
      
      // Check for game ending conditions
      const status = {
        isOver: false,
        result: null,
        reason: null
      };

      // Check for checkmate
      if (chess.isCheckmate()) {
        status.isOver = true;
        status.result = chess.turn() === 'w' ? '0-1' : '1-0';
        status.reason = 'Checkmate';
      } 
      // Check for stalemate
      else if (chess.isStalemate()) {
        status.isOver = true;
        status.result = '½-½';
        status.reason = 'Stalemate';
      }
      // Check for draw by insufficient material
      else if (chess.isInsufficientMaterial()) {
        status.isOver = true;
        status.result = '½-½'; 
        status.reason = 'Insufficient material';
      }
      // Check for draw by threefold repetition
      else if (chess.isThreefoldRepetition()) {
        status.isOver = true;
        status.result = '½-½';
        status.reason = 'Threefold repetition';
      }
      // Check for draw by 50-move rule
      else if (chess.isDraw()) {
        status.isOver = true;
        status.result = '½-½';
        status.reason = 'Draw';
      }
      // Check if in check
      else if (chess.isCheck()) {
        status.reason = 'Check';
      }

      setGameStatus(status);
    } catch (err) {
      console.error('Error updating board position:', err);
    }
  };

  const updateGameResult = async (result) => {
    try {
      console.log(`Attempting to update game ${id} result to: ${result}`);
      
      const response = await fetch(`/api/games/${id}/update-result`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ result }),
      });
      
      console.log(`Response status: ${response.status}`);
      const responseData = await response.json();
      console.log('Response data:', responseData);
      
      if (!response.ok) {
        throw new Error(`Failed to update game result: ${responseData.message || 'Unknown error'}`);
      }
      
      // Update local state
      setGame(prevGame => ({
        ...prevGame,
        result
      }));
      
      console.log(`Game result updated to ${result}`);
    } catch (err) {
      console.error('Error updating game result:', err);
      setError(`Failed to update game result: ${err.message}`);
    }
  };

  // Add useEffect to detect game over and update result
  useEffect(() => {
    // Only update result if game is currently in progress and now detected as over
    if (game && game.result === '*' && gameStatus.isOver && gameStatus.result) {
      updateGameResult(gameStatus.result);
    }
  }, [gameStatus, game?.result]);

  const fetchGameData = async () => {
    try {
      setLoading(true);
      const response = await fetch(`/api/games/${id}`);
      if (!response.ok) {
        throw new Error('Failed to fetch game data');
      }
      const data = await response.json();
      setGame(data.game);
      setLoading(false);
    } catch (err) {
      console.error('Error fetching game data:', err);
      setError('Failed to load game data. Please try again later.');
      setLoading(false);
    }
  };

  const fetchSerialPorts = async () => {
    try {
      const response = await fetch('/api/serial/ports');
      if (!response.ok) {
        throw new Error('Failed to fetch serial ports');
      }
      const data = await response.json();
      setPorts(data.ports || []);
      if (data.ports && data.ports.length > 0) {
        setSelectedPort(data.ports[0].device);
      }
    } catch (err) {
      console.error('Error fetching serial ports:', err);
      setError('Failed to load serial ports. Please try again later.');
    }
  };

  const fetchActiveGameState = async () => {
    if (!id || !connected) return;
    
    try {
      const response = await fetch(`/api/games/${id}/state`);
      if (!response.ok) {
        // If we get a 400, the game is no longer active
        if (response.status === 400) {
          setConnected(false);
          return;
        }
        throw new Error('Failed to fetch active game state');
      }
      const data = await response.json();
      setGame(data.game);
      
      // Update board if we're viewing the latest move
      if (selectedMoveIndex === (game?.moves?.length - 1)) {
        updateBoardPosition(data.game.moves.length - 1);
      }
    } catch (err) {
      console.error('Error fetching active game state:', err);
      // Don't set error message for polling errors to avoid UI noise
    }
  };

  const handleConnect = async () => {
    if (!selectedPort) {
      setError('Please select a serial port');
      return;
    }
    
    // Don't attempt to connect if game is already completed
    if (game.result !== '*') {
      setError('Cannot connect to a completed game. Only in-progress games can be connected to.');
      return;
    }
    
    try {
      const response = await fetch('/api/serial/connect', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          port: selectedPort,
          game_id: id,
          baud_rate: 115200
        }),
      });
      
      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || 'Failed to connect to serial port');
      }
      
      setConnected(true);
      setError(null);
    } catch (err) {
      console.error('Error connecting to serial port:', err);
      setError(err.message || 'Failed to connect to serial port');
    }
  };

  const handleDisconnect = async () => {
    try {
      const response = await fetch('/api/serial/disconnect', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        }
      });
      
      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || 'Failed to disconnect from serial port');
      }
      
      setConnected(false);
      // Refresh game data after disconnect to get the final saved state
      fetchGameData();
    } catch (err) {
      console.error('Error disconnecting from serial port:', err);
      setError(err.message || 'Failed to disconnect from serial port');
    }
  };

  const handleFlipBoard = () => {
    setBoardOrientation(boardOrientation === 'white' ? 'black' : 'white');
  };

  const handleMoveClick = (index) => {
    updateBoardPosition(index);
  };
  
  const generatePGN = () => {
    if (!game || !game.moves || game.moves.length <= 1) {
      return '';
    }
    
    // Create a new chess.js instance to build the game
    const chess = new Chess();
    
    // Add header information
    const headers = [
      ['Event', game.event || '?'],
      ['Site', game.site || '?'],
      ['Date', game.date || '?'],
      ['Round', game.round || '?'],
      ['White', game.white || '?'],
      ['Black', game.black || '?'],
      ['Result', game.result || '*']
    ];
    
    // Skip the first move (initial position)
    for (let i = 1; i < game.moves.length; i++) {
      const move = game.moves[i];
      
      // Skip illegal or missing moves
      if (!move.algebraic || move.is_legal === false) {
        continue;
      }
      
      try {
        // Add the move to the chess instance
        chess.move(move.algebraic);
      } catch (err) {
        console.error(`Error adding move ${move.algebraic} to PGN:`, err);
        // Continue with the next move if one fails
      }
    }
    
    // Set header information
    headers.forEach(([key, value]) => {
      chess.header(key, value);
    });
    
    // Generate PGN string
    return chess.pgn();
  };
  
  const downloadPGN = () => {
    const pgn = generatePGN();
    if (!pgn) {
      setError('No moves available to export');
      return;
    }
    
    // Create a blob with the PGN content
    const blob = new Blob([pgn], { type: 'text/plain' });
    const url = URL.createObjectURL(blob);
    
    // Create a temporary link element to trigger the download
    const a = document.createElement('a');
    a.href = url;
    a.download = `${game.white}-vs-${game.black}-${game.date}.pgn`.replace(/\s+/g, '_');
    document.body.appendChild(a);
    a.click();
    
    // Clean up
    setTimeout(() => {
      document.body.removeChild(a);
      URL.revokeObjectURL(url);
    }, 100);
  };

  if (loading) {
    return (
      <div className="min-h-screen flex items-center justify-center bg-gray-50">
        <div className="text-center">
          <h1 className="text-2xl font-bold mb-4 text-gray-800">ChessLink</h1>
          <p className="text-gray-600">Loading game...</p>
        </div>
      </div>
    );
  }

  if (!game) {
    return (
      <div className="min-h-screen flex items-center justify-center bg-gray-50">
        <div className="text-center">
          <h1 className="text-2xl font-bold mb-4 text-gray-800">ChessLink</h1>
          <p className="text-gray-600">Game not found</p>
          <Link href="/" className="text-blue-600 hover:underline mt-4 inline-block">
            Back to Games
          </Link>
        </div>
      </div>
    );
  }

  return (
    <div className="min-h-screen bg-gray-50 py-4">
      <div className="container mx-auto px-4 max-w-7xl">
        <div className="flex justify-between items-center mb-4">
          <h1 className="text-2xl font-bold text-gray-800">ChessLink Game</h1>
          <Link href="/" className="text-blue-600 hover:underline">
            Back to Games
          </Link>
        </div>
        
        {error && (
          <div className="bg-red-100 border border-red-400 text-red-700 px-4 py-2 rounded mb-3 text-sm">
            {error}
          </div>
        )}
        
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-4">
          {/* Left column - Chess board and controls */}
          <div>
            <div className="bg-white shadow-md rounded-lg overflow-hidden mb-4">
              <div className="p-3 bg-gray-100 border-b border-gray-200">
                <div className="flex justify-between items-center">
                  <h2 className="text-lg font-semibold text-gray-800">Chess Board</h2>
                  <div>
                    <button
                      onClick={handleFlipBoard}
                      className="bg-blue-600 hover:bg-blue-700 text-white font-bold py-1 px-3 rounded shadow text-sm"
                    >
                      Flip Board
                    </button>
                  </div>
                </div>
              </div>
              <div className="p-3">
                <div className="aspect-square w-full mb-2">
                  {chessInstance && (
                    <Chessboard 
                      position={chessInstance.fen()}
                      boardOrientation={boardOrientation}
                      arePiecesDraggable={false}
                    />
                  )}
                </div>
                {gameStatus.isOver && (
                  <div className={`p-2 text-center font-bold mb-2 rounded ${
                    gameStatus.result === '1-0' ? 'bg-green-100 text-green-800' :
                    gameStatus.result === '0-1' ? 'bg-red-100 text-red-800' :
                    'bg-blue-100 text-blue-800'
                  }`}>
                    Game Over: {gameStatus.result} - {gameStatus.reason}
                  </div>
                )}
                {!gameStatus.isOver && gameStatus.reason === 'Check' && (
                  <div className="p-1 text-center font-bold mb-2 rounded bg-yellow-100 text-yellow-800">
                    Check!
                  </div>
                )}
                <div className="text-center text-sm text-gray-700">
                  {game.moves && game.moves[selectedMoveIndex] && (
                    <div>
                      <p className="font-medium">
                        {selectedMoveIndex === 0 
                          ? 'Initial Position' 
                          : `Move ${Math.ceil(selectedMoveIndex / 2)}: ${game.moves[selectedMoveIndex].algebraic || '[unknown]'}`
                        }
                      </p>
                      <p className="text-gray-500 text-xs mt-1">
                        {selectedMoveIndex === 0 
                          ? "White's turn" 
                          : `${game.moves[selectedMoveIndex].player}'s turn`
                        }
                      </p>
                    </div>
                  )}
                </div>
                <div className="flex justify-center gap-1 mt-2">
                  <button
                    onClick={() => updateBoardPosition(0)}
                    disabled={selectedMoveIndex === 0}
                    className={`${selectedMoveIndex === 0 ? 'bg-gray-400 cursor-not-allowed' : 'bg-gray-600 hover:bg-gray-700'} text-white text-xs py-1 px-2 rounded shadow`}
                  >
                    First
                  </button>
                  <button
                    onClick={() => updateBoardPosition(selectedMoveIndex - 1)}
                    disabled={selectedMoveIndex === 0}
                    className={`${selectedMoveIndex === 0 ? 'bg-gray-400 cursor-not-allowed' : 'bg-gray-600 hover:bg-gray-700'} text-white text-xs py-1 px-2 rounded shadow`}
                  >
                    Prev
                  </button>
                  <button
                    onClick={() => updateBoardPosition(selectedMoveIndex + 1)}
                    disabled={!game.moves || selectedMoveIndex >= game.moves.length - 1}
                    className={`${!game.moves || selectedMoveIndex >= game.moves.length - 1 ? 'bg-gray-400 cursor-not-allowed' : 'bg-gray-600 hover:bg-gray-700'} text-white text-xs py-1 px-2 rounded shadow`}
                  >
                    Next
                  </button>
                  <button
                    onClick={() => updateBoardPosition(game.moves ? game.moves.length - 1 : 0)}
                    disabled={!game.moves || selectedMoveIndex >= game.moves.length - 1}
                    className={`${!game.moves || selectedMoveIndex >= game.moves.length - 1 ? 'bg-gray-400 cursor-not-allowed' : 'bg-gray-600 hover:bg-gray-700'} text-white text-xs py-1 px-2 rounded shadow`}
                  >
                    Last
                  </button>
                </div>
              </div>
            </div>
            
            {/* Game Information */}
            <div className="bg-white shadow-md rounded-lg overflow-hidden mb-4">
              <div className="p-3 bg-gray-100 border-b border-gray-200">
                <div className="flex justify-between items-center">
                  <h2 className="text-lg font-semibold text-gray-800">Game Information</h2>
                  <button
                    onClick={downloadPGN}
                    className="bg-green-600 hover:bg-green-700 text-white font-bold py-1 px-3 rounded shadow text-sm"
                    disabled={!game.moves || game.moves.length <= 1}
                  >
                    Export to PGN
                  </button>
                </div>
              </div>
              <div className="p-3">
                <div className="grid grid-cols-2 gap-2 text-sm">
                  <div>
                    <p className="font-medium text-gray-700">Event:</p>
                    <p className="text-gray-800">{game.event}</p>
                  </div>
                  <div>
                    <p className="font-medium text-gray-700">Date:</p>
                    <p className="text-gray-800">{game.date}</p>
                  </div>
                  <div>
                    <p className="font-medium text-gray-700">White:</p>
                    <p className="text-gray-800">{game.white}</p>
                  </div>
                  <div>
                    <p className="font-medium text-gray-700">Black:</p>
                    <p className="text-gray-800">{game.black}</p>
                  </div>
                  <div>
                    <p className="font-medium text-gray-700">Result:</p>
                    <p className="text-gray-800">{game.result === '*' ? 'In progress' : game.result}</p>
                  </div>
                  <div>
                    <p className="font-medium text-gray-700">Total Moves:</p>
                    <p className="text-gray-800">{game.moves ? game.moves.length - 1 : 0}</p>
                  </div>
                </div>
                
                {/* Manual Result Update Section */}
                <div className="mt-3 pt-2 border-t border-gray-200">
                  <div className="flex items-center justify-between mb-1">
                    <h3 className="font-medium text-gray-700 text-sm">Manual Result Update</h3>
                    <div className="flex gap-1">
                      <button
                        onClick={() => updateGameResult('1-0')}
                        className="bg-white border border-gray-300 hover:bg-gray-50 text-gray-800 font-medium py-0.5 px-2 rounded text-xs"
                      >
                        1-0
                      </button>
                      <button
                        onClick={() => updateGameResult('0-1')}
                        className="bg-white border border-gray-300 hover:bg-gray-50 text-gray-800 font-medium py-0.5 px-2 rounded text-xs"
                      >
                        0-1
                      </button>
                      <button
                        onClick={() => updateGameResult('½-½')}
                        className="bg-white border border-gray-300 hover:bg-gray-50 text-gray-800 font-medium py-0.5 px-2 rounded text-xs"
                      >
                        ½-½
                      </button>
                      <button
                        onClick={() => updateGameResult('*')}
                        className="bg-white border border-gray-300 hover:bg-gray-50 text-gray-800 font-medium py-0.5 px-2 rounded text-xs"
                      >
                        *
                      </button>
                    </div>
                  </div>
                  <p className="text-xs text-gray-500">
                    Use these buttons to manually update the game result.
                  </p>
                </div>
              </div>
            </div>
          </div>
          
          {/* Right column - Serial connection and move list */}
          <div>
            {/* Serial Port Connection */}
            <div className="bg-white shadow-md rounded-lg overflow-hidden mb-4">
              <div className="p-3 bg-gray-100 border-b border-gray-200">
                <h2 className="text-lg font-semibold text-gray-800">Serial Connection</h2>
              </div>
              <div className="p-3">
                {connected ? (
                  <div>
                    <div className="mb-2 p-2 bg-green-50 border border-green-200 text-green-700 rounded text-sm">
                      Connected to serial port: {selectedPort}
                    </div>
                    <button 
                      onClick={handleDisconnect}
                      className="bg-red-600 hover:bg-red-700 text-white font-bold py-1 px-3 rounded shadow text-sm"
                    >
                      Disconnect
                    </button>
                  </div>
                ) : game.result !== '*' ? (
                  <div className="p-2 bg-yellow-50 border border-yellow-200 text-yellow-700 rounded text-sm">
                    Cannot connect to a completed game with result: {game.result}.
                  </div>
                ) : (
                  <div className="flex gap-2 items-end">
                    <div className="flex-1">
                      <label className="block text-xs font-medium text-gray-700 mb-1">Select Serial Port</label>
                      <select
                        className="w-full px-2 py-1 border border-gray-300 rounded shadow-sm focus:outline-none focus:ring-1 focus:ring-blue-500 focus:border-blue-500 text-sm"
                        value={selectedPort}
                        onChange={(e) => setSelectedPort(e.target.value)}
                      >
                        {ports.length === 0 && (
                          <option value="">No ports available</option>
                        )}
                        {ports.map((port) => (
                          <option key={port.device} value={port.device}>
                            {port.device} - {port.description || 'Unknown'}
                          </option>
                        ))}
                      </select>
                    </div>
                    <div>
                      <button 
                        onClick={handleConnect}
                        disabled={!selectedPort}
                        className={`${!selectedPort ? 'bg-gray-400 cursor-not-allowed' : 'bg-blue-600 hover:bg-blue-700'} text-white font-bold py-1 px-3 rounded shadow text-sm`}
                      >
                        Connect
                      </button>
                    </div>
                    <div>
                      <button 
                        onClick={fetchSerialPorts}
                        className="bg-gray-600 hover:bg-gray-700 text-white font-bold py-1 px-3 rounded shadow text-sm"
                      >
                        Refresh
                      </button>
                    </div>
                  </div>
                )}
              </div>
            </div>

            {/* Game Moves */}
            <div className="bg-white shadow-md rounded-lg overflow-hidden">
              <div className="p-3 bg-gray-100 border-b border-gray-200">
                <div className="flex justify-between items-center">
                  <h2 className="text-lg font-semibold text-gray-800">Game Moves</h2>
                  {connected && (
                    <div className="text-xs text-green-600 font-medium">
                      Live updating...
                    </div>
                  )}
                </div>
              </div>
              <div className="p-2">
                {(!game.moves || game.moves.length <= 1) ? (
                  <div className="text-center text-gray-500 py-2 text-sm">
                    No moves recorded yet.
                  </div>
                ) : (
                  <div className="overflow-x-auto overflow-y-auto" style={{ maxHeight: 'calc(100vh - 26rem)', minHeight: '200px' }}>
                    <table className="min-w-full divide-y divide-gray-200">
                      <thead className="bg-gray-50 sticky top-0 z-10">
                        <tr>
                          <th className="px-3 py-2 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                            #
                          </th>
                          <th className="px-3 py-2 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                            Move
                          </th>
                          <th className="px-3 py-2 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                            Player
                          </th>
                          <th className="px-3 py-2 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                            Algebraic
                          </th>
                          <th className="px-3 py-2 text-left text-xs font-medium text-gray-500 uppercase tracking-wider">
                            Legal
                          </th>
                        </tr>
                      </thead>
                      <tbody className="bg-white divide-y divide-gray-200">
                        {game.moves.map((move, index) => (
                          <tr 
                            key={move.move_id} 
                            className={`hover:bg-gray-50 cursor-pointer ${index === selectedMoveIndex ? 'bg-blue-50' : ''}`}
                            onClick={() => handleMoveClick(index)}
                          >
                            <td className="px-3 py-2 whitespace-nowrap text-xs text-gray-500">
                              {index}
                            </td>
                            <td className="px-3 py-2 whitespace-nowrap text-xs font-medium text-gray-700">
                              {index === 0 ? 'Initial' : `Move ${Math.ceil(index / 2)}`}
                            </td>
                            <td className="px-3 py-2 whitespace-nowrap text-xs text-gray-500">
                              {move.player || '-'}
                            </td>
                            <td className="px-3 py-2 whitespace-nowrap text-xs text-gray-500">
                              {move.algebraic || '-'}
                            </td>
                            <td className="px-3 py-2 whitespace-nowrap">
                              {move.is_legal === true && (
                                <span className="px-1.5 inline-flex text-xs leading-5 font-semibold rounded-full bg-green-100 text-green-800">
                                  Legal
                                </span>
                              )}
                              {move.is_legal === false && (
                                <span className="px-1.5 inline-flex text-xs leading-5 font-semibold rounded-full bg-red-100 text-red-800">
                                  Illegal
                                </span>
                              )}
                              {move.is_legal === null && (
                                <span className="px-1.5 inline-flex text-xs leading-5 font-semibold rounded-full bg-gray-100 text-gray-800">
                                  Unknown
                                </span>
                              )}
                            </td>
                          </tr>
                        ))}
                      </tbody>
                    </table>
                  </div>
                )}
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  );
} 