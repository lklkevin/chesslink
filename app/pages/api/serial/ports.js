export default async function handler(req, res) {
  const { method } = req;
  const baseURL = 'http://127.0.0.1:5000';

  try {
    // Forward GET request to Flask
    if (method === 'GET') {
      const response = await fetch(`${baseURL}/serial/ports`);
      const data = await response.json();
      return res.status(response.status).json(data);
    } else {
      return res.status(405).json({ message: 'Method not allowed' });
    }
  } catch (error) {
    console.error('Error forwarding request to Flask backend:', error);
    return res.status(500).json({ 
      message: 'Internal server error'
    });
  }
} 