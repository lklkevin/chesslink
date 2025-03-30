/** @type {import('next').NextConfig} */
const nextConfig = {
  reactStrictMode: true,
  async rewrites() {
    return [
      {
        source: '/api/:path*',
        destination: 'http://127.0.0.1:5000/:path*',
      },
    ];
  },
  // Force Next.js server to listen on IPv4
  server: {
    listen: {
      host: '0.0.0.0',
      port: 3000,
    },
  }
};

export default nextConfig;
