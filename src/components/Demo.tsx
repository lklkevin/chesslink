import React, { useState, useEffect } from 'react';
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardDescription, CardFooter, CardHeader, CardTitle } from "@/components/ui/card";
import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { Badge } from "@/components/ui/badge";
import { Tooltip, TooltipContent, TooltipProvider, TooltipTrigger } from "@/components/ui/tooltip";

// Mock FEN positions for demo
const DEMO_POSITIONS = [
  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", // Starting position
  "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1", // After 1. e4
  "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 2", // After 1... e5
  "rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2", // After 2. Nf3
];

// Chess piece SVG mappings 
const PIECE_IMAGES: Record<string, string> = {
  'p': 'https://www.chess.com/chess-themes/pieces/neo/150/bp.png',
  'n': 'https://www.chess.com/chess-themes/pieces/neo/150/bn.png',
  'b': 'https://www.chess.com/chess-themes/pieces/neo/150/bb.png',
  'r': 'https://www.chess.com/chess-themes/pieces/neo/150/br.png',
  'q': 'https://www.chess.com/chess-themes/pieces/neo/150/bq.png',
  'k': 'https://www.chess.com/chess-themes/pieces/neo/150/bk.png',
  'P': 'https://www.chess.com/chess-themes/pieces/neo/150/wp.png',
  'N': 'https://www.chess.com/chess-themes/pieces/neo/150/wn.png',
  'B': 'https://www.chess.com/chess-themes/pieces/neo/150/wb.png',
  'R': 'https://www.chess.com/chess-themes/pieces/neo/150/wr.png',
  'Q': 'https://www.chess.com/chess-themes/pieces/neo/150/wq.png',
  'K': 'https://www.chess.com/chess-themes/pieces/neo/150/wk.png',
};

const Demo: React.FC = () => {
  const [currentFen, setCurrentFen] = useState(DEMO_POSITIONS[0]);
  const [moveIndex, setMoveIndex] = useState(0);
  const [isConnected, setIsConnected] = useState(false);
  const [activeSensors, setActiveSensors] = useState<number[]>([]);
  const [boardState, setBoardState] = useState<string[][]>(Array(8).fill(Array(8).fill('')));
  const [dataFlow, setDataFlow] = useState<string[]>([]);
  const [isSimulating, setIsSimulating] = useState(false);
  const [isExpanded, setIsExpanded] = useState(false);
  const [pieceTheme, setPieceTheme] = useState<'neo' | 'cburnett' | 'classic'>('neo');

  // Parse FEN string to board state
  useEffect(() => {
    const parseFen = (fen: string) => {
      const board = Array(8).fill(0).map(() => Array(8).fill(''));
      const fenParts = fen.split(' ');
      const rows = fenParts[0].split('/');
      
      rows.forEach((row, rowIndex) => {
        let colIndex = 0;
        for (let i = 0; i < row.length; i++) {
          const char = row[i];
          if (/\d/.test(char)) {
            colIndex += parseInt(char, 10);
          } else {
            board[rowIndex][colIndex] = char;
            colIndex += 1;
          }
        }
      });
      
      setBoardState(board);
    };
    
    parseFen(currentFen);
  }, [currentFen]);

  // Simulate sensor activation
  const simulateSensorActivation = (row: number, col: number) => {
    setActiveSensors(prev => [...prev, row * 8 + col]);
    
    // Clear sensor activation after 1 second
    setTimeout(() => {
      setActiveSensors(prev => prev.filter(sensor => sensor !== row * 8 + col));
    }, 1000);
  };

  // Add log entry to data flow
  const addLogEntry = (entry: string) => {
    setDataFlow(prev => [entry, ...prev].slice(0, 10));
  };

  // Simulate a move
  const simulateMove = () => {
    if (isSimulating) return;
    
    setIsSimulating(true);
    
    const nextIndex = (moveIndex + 1) % DEMO_POSITIONS.length;
    
    // Simulate physical move based on FEN changes
    const currentFenParts = DEMO_POSITIONS[moveIndex].split(' ');
    const nextFenParts = DEMO_POSITIONS[nextIndex].split(' ');
    
    // Log sensor detection
    addLogEntry("Sensor detecting piece movement...");
    
    // For demo, just highlight a random square as the source and target
    const sourceRow = Math.floor(Math.random() * 8);
    const sourceCol = Math.floor(Math.random() * 8);
    simulateSensorActivation(sourceRow, sourceCol);
    
    setTimeout(() => {
      addLogEntry("Processing sensor data...");
      
      setTimeout(() => {
        const targetRow = Math.floor(Math.random() * 8);
        const targetCol = Math.floor(Math.random() * 8);
        simulateSensorActivation(targetRow, targetCol);
        
        addLogEntry("Board state change detected");
        
        setTimeout(() => {
          addLogEntry("Converting to FEN notation");
          
          setTimeout(() => {
            addLogEntry(`Move validated: ${currentFenParts[1] === 'w' ? 'White' : 'Black'} moved`);
            setCurrentFen(DEMO_POSITIONS[nextIndex]);
            setMoveIndex(nextIndex);
            
            setTimeout(() => {
              addLogEntry("Updated state sent to software application");
              setIsSimulating(false);
            }, 500);
          }, 500);
        }, 500);
      }, 500);
    }, 500);
  };

  // Toggle connection status
  const toggleConnection = () => {
    setIsConnected(!isConnected);
    addLogEntry(isConnected ? "Disconnected from ChessLink board" : "Connected to ChessLink board");
  };

  // Reset demo
  const resetDemo = () => {
    setCurrentFen(DEMO_POSITIONS[0]);
    setMoveIndex(0);
    setActiveSensors([]);
    setDataFlow([]);
    addLogEntry("Demo reset to initial state");
  };

  // Toggle board expansion
  const toggleBoardExpansion = () => {
    setIsExpanded(!isExpanded);
    addLogEntry(isExpanded ? "Board view collapsed" : "Board view expanded");
  };

  // Get rank and file labels
  const getRankLabel = (rank: number) => 8 - rank;
  const getFileLabel = (file: number) => String.fromCharCode(97 + file);

  return (
    <section id="demo" className="py-16 bg-gray-50">
      <div className="container mx-auto px-4">
        <h2 className="text-3xl font-bold text-center mb-12">Interactive ChessLink Demo</h2>
        
        <div className={`grid grid-cols-1 ${isExpanded ? 'lg:grid-cols-1' : 'lg:grid-cols-12'} gap-8`}>
          {/* Left column with chess board visualization */}
          <div className={`${isExpanded ? 'max-w-4xl mx-auto w-full' : 'lg:col-span-7'}`}>
            <Card className="shadow-xl">
              <CardHeader className="bg-gradient-to-r from-gray-50 to-gray-100 border-b">
                <CardTitle className="flex items-center justify-between">
                  ChessLink Board
                  <div className="flex items-center space-x-2">
                    <Badge variant={isConnected ? "default" : "destructive"}>
                      {isConnected ? "Connected" : "Disconnected"}
                    </Badge>
                    <Button 
                      variant="outline" 
                      size="sm"
                      onClick={toggleBoardExpansion}
                      className="ml-2"
                    >
                      {isExpanded ? "Collapse" : "Expand"} Board
                    </Button>
                  </div>
                </CardTitle>
                <CardDescription>
                  Interactive visualization of the smart chess board
                </CardDescription>
              </CardHeader>
              <CardContent className="p-6">
                <div className={`relative ${isExpanded ? 'max-w-3xl mx-auto' : ''}`}>
                  {/* Files (A-H) labels at the top */}
                  <div className="grid grid-cols-8 pl-6 pr-6 mb-1">
                    {Array(8).fill(0).map((_, i) => (
                      <div key={`file-top-${i}`} className="flex justify-center items-center h-6 text-sm font-medium text-gray-500">
                        {getFileLabel(i)}
                      </div>
                    ))}
                  </div>
                
                  <div className="flex">
                    {/* Ranks (1-8) labels on the left */}
                    <div className="flex flex-col justify-around pr-2">
                      {Array(8).fill(0).map((_, i) => (
                        <div key={`rank-left-${i}`} className="flex justify-center items-center w-4 h-full text-sm font-medium text-gray-500">
                          {getRankLabel(i)}
                        </div>
                      ))}
                    </div>
                    
                    {/* Main chessboard */}
                    <div className={`aspect-square w-full border-2 border-gray-700 rounded-sm shadow-lg ${isExpanded ? 'max-h-[70vh]' : ''}`}>
                      <div className="grid grid-cols-8 grid-rows-8 h-full w-full">
                        {boardState.flat().map((piece, index) => {
                          const row = Math.floor(index / 8);
                          const col = index % 8;
                          const isBlackSquare = (row + col) % 2 === 1;
                          const isActiveSensor = activeSensors.includes(index);
                          const squareNotation = `${getFileLabel(col)}${getRankLabel(row)}`;
                          
                          return (
                            <div 
                              key={index}
                              className={`
                                relative flex items-center justify-center
                                ${isBlackSquare ? 'bg-[#b58863]' : 'bg-[#f0d9b5]'}
                                ${isActiveSensor ? 'ring-4 ring-blue-500 ring-inset' : ''}
                                transition-all duration-150 hover:brightness-110
                              `}
                            >
                              {piece && (
                                <div className="absolute inset-0 flex items-center justify-center">
                                  <img 
                                    src={PIECE_IMAGES[piece]} 
                                    alt={`${piece === piece.toUpperCase() ? 'White' : 'Black'} ${
                                      piece.toLowerCase() === 'p' ? 'Pawn' : 
                                      piece.toLowerCase() === 'r' ? 'Rook' : 
                                      piece.toLowerCase() === 'n' ? 'Knight' : 
                                      piece.toLowerCase() === 'b' ? 'Bishop' : 
                                      piece.toLowerCase() === 'q' ? 'Queen' : 'King'
                                    }`}
                                    className={`w-full h-full object-contain p-1 ${isActiveSensor ? 'brightness-110' : ''}`}
                                    style={{ filter: isActiveSensor ? 'drop-shadow(0 0 3px rgba(59, 130, 246, 0.7))' : 'none' }}
                                  />
                                </div>
                              )}
                              {isActiveSensor && (
                                <div className="absolute inset-0 bg-blue-500 opacity-20 z-10"></div>
                              )}
                              {/* Square coordinates in corner */}
                              <div className="absolute bottom-0.5 right-0.5 text-[8px] opacity-50">
                                {squareNotation}
                              </div>
                            </div>
                          );
                        })}
                      </div>
                    </div>
                    
                    {/* Ranks (1-8) labels on the right */}
                    <div className="flex flex-col justify-around pl-2">
                      {Array(8).fill(0).map((_, i) => (
                        <div key={`rank-right-${i}`} className="flex justify-center items-center w-4 h-full text-sm font-medium text-gray-500">
                          {getRankLabel(i)}
                        </div>
                      ))}
                    </div>
                  </div>
                  
                  {/* Files (A-H) labels at the bottom */}
                  <div className="grid grid-cols-8 pl-6 pr-6 mt-1">
                    {Array(8).fill(0).map((_, i) => (
                      <div key={`file-bottom-${i}`} className="flex justify-center items-center h-6 text-sm font-medium text-gray-500">
                        {getFileLabel(i)}
                      </div>
                    ))}
                  </div>
                </div>
                
                <div className="text-center text-sm text-gray-500 mt-4 mb-4">
                  <span className="mr-2">FEN:</span>
                  <code className="bg-gray-100 p-1 px-2 rounded text-xs font-mono tracking-tight">{currentFen}</code>
                </div>
                
                <div className="flex flex-wrap gap-3 justify-center mt-6">
                  <Button 
                    onClick={toggleConnection}
                    className={`${isConnected ? 'bg-green-600 hover:bg-green-700' : 'bg-blue-600 hover:bg-blue-700'}`}
                  >
                    {isConnected ? "Disconnect Board" : "Connect Board"}
                  </Button>
                  <Button 
                    variant="outline" 
                    onClick={simulateMove}
                    disabled={!isConnected || isSimulating}
                    className="border-gray-300"
                  >
                    Simulate Move
                  </Button>
                  <Button 
                    variant="outline" 
                    onClick={resetDemo}
                    className="border-gray-300"
                  >
                    Reset Demo
                  </Button>
                </div>
              </CardContent>
            </Card>
          </div>
          
          {/* Right column with system info */}
          <div className={`${isExpanded ? 'max-w-4xl mx-auto w-full' : 'lg:col-span-5'}`}>
            <Tabs defaultValue="data-flow" className="w-full">
              <TabsList className="grid grid-cols-3">
                <TabsTrigger value="data-flow">Data Flow</TabsTrigger>
                <TabsTrigger value="hardware">Hardware</TabsTrigger>
                <TabsTrigger value="software">Software</TabsTrigger>
              </TabsList>
              
              {/* Data Flow Tab */}
              <TabsContent value="data-flow">
                <Card>
                  <CardHeader className="bg-gray-50 border-b">
                    <CardTitle>System Data Flow</CardTitle>
                    <CardDescription>
                      Visualize how data moves through the ChessLink system
                    </CardDescription>
                  </CardHeader>
                  <CardContent className="p-4">
                    <div className="border rounded-md p-3 bg-black text-green-400 font-mono h-80 overflow-y-auto space-y-2">
                      {dataFlow.length > 0 ? (
                        dataFlow.map((entry, i) => (
                          <div key={i} className="flex">
                            <span className="opacity-60 mr-2">[{new Date().toLocaleTimeString()}]</span>
                            <span>{entry}</span>
                          </div>
                        ))
                      ) : (
                        <div className="text-center opacity-60 h-full flex items-center justify-center">
                          {isConnected 
                            ? "Connected and waiting for activity..." 
                            : "Connect the board to see data flow"}
                        </div>
                      )}
                    </div>
                  </CardContent>
                </Card>
              </TabsContent>
              
              {/* Hardware Tab */}
              <TabsContent value="hardware">
                <Card>
                  <CardHeader className="bg-gray-50 border-b">
                    <CardTitle>Hardware Components</CardTitle>
                    <CardDescription>
                      The physical components of the ChessLink system
                    </CardDescription>
                  </CardHeader>
                  <CardContent className="space-y-4 p-4">
                    <div className="grid grid-cols-2 gap-4">
                      <div className="border rounded p-4 shadow-sm">
                        <h4 className="font-medium text-lg mb-2">Sensor Array</h4>
                        <p className="text-sm text-gray-600">64 IR sensors with RGB LEDs arranged in an 8×8 grid</p>
                      </div>
                      <div className="border rounded p-4 shadow-sm">
                        <h4 className="font-medium text-lg mb-2">ESP32-C3</h4>
                        <p className="text-sm text-gray-600">Central microcontroller running at 160MHz</p>
                      </div>
                      <div className="border rounded p-4 shadow-sm">
                        <h4 className="font-medium text-lg mb-2">Arduino Nano</h4>
                        <p className="text-sm text-gray-600">Co-processor connected via I2C interface</p>
                      </div>
                      <div className="border rounded p-4 shadow-sm">
                        <h4 className="font-medium text-lg mb-2">Connectivity</h4>
                        <p className="text-sm text-gray-600">WiFi, Bluetooth 5.0, Serial connection options</p>
                      </div>
                    </div>
                    
                    <div className="border-t pt-4">
                      <h4 className="font-medium text-lg mb-2">System Diagram</h4>
                      <div className="border-2 border-dashed border-gray-300 p-4 rounded bg-gray-50 text-center h-40 flex items-center justify-center">
                        <p className="text-gray-500">Interactive system diagram would be displayed here</p>
                      </div>
                    </div>
                  </CardContent>
                </Card>
              </TabsContent>
              
              {/* Software Tab */}
              <TabsContent value="software">
                <Card>
                  <CardHeader className="bg-gray-50 border-b">
                    <CardTitle>Software Architecture</CardTitle>
                    <CardDescription>
                      The software components and data processing pipeline
                    </CardDescription>
                  </CardHeader>
                  <CardContent className="p-4">
                    <div className="space-y-4">
                      <div className="border rounded p-4 shadow-sm">
                        <h4 className="font-medium text-lg mb-2">Local Board State Management</h4>
                        <ul className="list-disc pl-5 text-sm text-gray-600">
                          <li>Maintains current board state</li>
                          <li>Tracks move history</li>
                          <li>Validates moves based on chess rules</li>
                          <li>Updates FEN notation</li>
                        </ul>
                      </div>
                      
                      <div className="border rounded p-4 shadow-sm">
                        <h4 className="font-medium text-lg mb-2">Data Processing Pipeline</h4>
                        <div className="flex flex-col space-y-2">
                          {[
                            "1. Sensor data collection",
                            "2. Board state determination",
                            "3. FEN notation conversion",
                            "4. Move validation",
                            "5. User interface updates",
                            "6. Storage and analysis (optional)"
                          ].map((step, i) => (
                            <div key={i} className="flex items-center">
                              <div className={`h-6 w-6 rounded-full mr-2 flex items-center justify-center text-xs ${
                                isConnected ? 'bg-green-500 text-white' : 'bg-gray-300'
                              }`}>
                                {i + 1}
                              </div>
                              <span className="text-sm">{step.split('. ')[1]}</span>
                            </div>
                          ))}
                        </div>
                      </div>
                    </div>
                  </CardContent>
                </Card>
              </TabsContent>
            </Tabs>
          </div>
        </div>
      </div>
    </section>
  );
};

export default Demo; 