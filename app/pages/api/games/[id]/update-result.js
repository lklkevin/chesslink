export default async function handler(req, res) {
  const { method } = req;
  const { id } = req.query;
  const baseURL = 'http://127.0.0.1:5000';

  try {
    // Forward POST request to Flask
    if (method === 'POST') {
      console.log(`API route: Forwarding update-result request for game ${id}`, req.body);
      
      const response = await fetch(`${baseURL}/games/${id}/update-result`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(req.body),
      });
      
      console.log(`API route: Flask response status: ${response.status}`);
      const data = await response.json();
      console.log('API route: Flask response data:', data);
      
      return res.status(response.status).json(data);
    } else {
      return res.status(405).json({ message: 'Method not allowed' });
    }
  } catch (error) {
    console.error('Error forwarding request to Flask backend:', error);
    return res.status(500).json({ 
      message: `Failed to connect to backend server: ${error.message}`
    });
  }
} 