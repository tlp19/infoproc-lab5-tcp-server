#!bin/bash
set -e

# Pull the latest version of the repository
echo "## Getting latest version from GitHub"
git pull origin master

# Compile the server
echo ""
echo "## Compiling tcp_server.cpp"
g++ tcp_server.cpp -o tcp_server

# Start the server on port 7000 (must be set as inbound rule on AWS/Azure)
echo ""
echo "Starting server on port 7000"
./tcp_server 7000
