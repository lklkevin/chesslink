import { useState, useEffect } from 'react';
import { useRouter } from 'next/router';
import Link from 'next/link';
import Image from "next/image";
import { Geist, Geist_Mono } from "next/font/google";

const geistSans = Geist({
  variable: "--font-geist-sans",
  subsets: ["latin"],
});

const geistMono = Geist_Mono({
  variable: "--font-geist-mono",
  subsets: ["latin"],
});

export default function Home() {
  const [games, setGames] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  const [showCreateForm, setShowCreateForm] = useState(false);
  const [newGame, setNewGame] = useState({
    event: 'Casual Game',
    site: 'ChessLink',
    white: 'White',
    black: 'Black'
  });
  
  const router = useRouter();

  useEffect(() => {
    fetchGames();
  }, []);

  const fetchGames = async () => {
    try {
      setLoading(true);
      const response = await fetch('/api/games');
      if (!response.ok) {
        throw new Error('Failed to fetch games');
      }
      const data = await response.json();
      setGames(data.games || []);
      setLoading(false);
    } catch (err) {
      console.error('Error fetching games:', err);
      setError('Failed to load games. Please try again later.');
      setLoading(false);
    }
  };

  const handleInputChange = (e) => {
    const { name, value } = e.target;
    setNewGame(prev => ({
      ...prev,
      [name]: value
    }));
  };

  const handleCreateGame = async (e) => {
    e.preventDefault();
    try {
      const response = await fetch('/api/games', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(newGame),
      });
      
      if (!response.ok) {
        throw new Error('Failed to create game');
      }
      
      const data = await response.json();
      // Navigate to the new game
      router.push(`/games/${data.game_id}`);
    } catch (err) {
      console.error('Error creating game:', err);
      setError('Failed to create game. Please try again.');
    }
  };

  if (loading) {
    return (
      <div className="min-h-screen flex items-center justify-center bg-gray-50">
        <div className="text-center">
          <h1 className="text-2xl font-bold mb-4 text-gray-800">ChessLink</h1>
          <p className="text-gray-600">Loading games...</p>
        </div>
      </div>
    );
  }

  return (
    <div className="min-h-screen bg-gray-50 py-8">
      <div className="container mx-auto px-4 max-w-4xl">
        <h1 className="text-3xl font-bold mb-6 text-center text-gray-800">ChessLink</h1>
        
        {error && (
          <div className="bg-red-100 border border-red-400 text-red-700 px-4 py-3 rounded mb-4">
            {error}
          </div>
        )}
        
        <div className="mb-6 flex justify-end">
          <button 
            onClick={() => setShowCreateForm(!showCreateForm)}
            className="bg-blue-600 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded shadow"
          >
            {showCreateForm ? 'Cancel' : 'Create New Game'}
          </button>
        </div>
        
        {showCreateForm && (
          <div className="bg-white p-6 rounded-lg shadow-md mb-6">
            <h2 className="text-xl font-semibold mb-4 text-gray-800">Create New Game</h2>
            <form onSubmit={handleCreateGame}>
              <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                <div>
                  <label className="block text-sm font-medium text-gray-700 mb-1">Event</label>
                  <input
                    type="text"
                    name="event"
                    value={newGame.event}
                    onChange={handleInputChange}
                    className="w-full px-3 py-2 border border-gray-300 rounded shadow-sm focus:outline-none focus:ring-2 focus:ring-blue-500 focus:border-blue-500"
                  />
                </div>
                <div>
                  <label className="block text-sm font-medium text-gray-700 mb-1">Site</label>
                  <input
                    type="text"
                    name="site"
                    value={newGame.site}
                    onChange={handleInputChange}
                    className="w-full px-3 py-2 border border-gray-300 rounded shadow-sm focus:outline-none focus:ring-2 focus:ring-blue-500 focus:border-blue-500"
                  />
                </div>
                <div>
                  <label className="block text-sm font-medium text-gray-700 mb-1">White Player</label>
                  <input
                    type="text"
                    name="white"
                    value={newGame.white}
                    onChange={handleInputChange}
                    className="w-full px-3 py-2 border border-gray-300 rounded shadow-sm focus:outline-none focus:ring-2 focus:ring-blue-500 focus:border-blue-500"
                  />
                </div>
                <div>
                  <label className="block text-sm font-medium text-gray-700 mb-1">Black Player</label>
                  <input
                    type="text"
                    name="black"
                    value={newGame.black}
                    onChange={handleInputChange}
                    className="w-full px-3 py-2 border border-gray-300 rounded shadow-sm focus:outline-none focus:ring-2 focus:ring-blue-500 focus:border-blue-500"
                  />
                </div>
              </div>
              <div className="mt-4 flex justify-end">
                <button type="submit" className="bg-green-600 hover:bg-green-700 text-white font-bold py-2 px-4 rounded shadow">
                  Create Game
                </button>
              </div>
            </form>
          </div>
        )}
        
        <div className="bg-white shadow-md rounded-lg overflow-hidden">
          <div className="p-4 bg-gray-100 border-b border-gray-200">
            <h2 className="text-xl font-semibold text-gray-800">Games</h2>
          </div>
          
          {games.length === 0 ? (
            <div className="p-6 text-center text-gray-500">
              No games found. Create your first game!
            </div>
          ) : (
            <div className="divide-y divide-gray-200">
              {games.map((game) => (
                <Link 
                  href={`/games/${game.game_id}`}
                  key={game.game_id}
                  className="block p-4 hover:bg-gray-50 transition"
                >
                  <div className="flex justify-between items-center">
                    <div>
                      <h3 className="font-medium text-gray-800">{game.white} vs {game.black}</h3>
                      <p className="text-sm text-gray-500">{game.date}</p>
                    </div>
                    <div className="text-sm">
                      {game.result === '*' ? (
                        <span className="text-blue-600 font-medium">In progress</span>
                      ) : (
                        <span className="text-gray-700">{game.result}</span>
                      )}
                    </div>
                  </div>
                </Link>
              ))}
            </div>
          )}
        </div>
      </div>
    </div>
  );
}
