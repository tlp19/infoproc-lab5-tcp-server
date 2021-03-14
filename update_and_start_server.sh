#!/bin/bash
set -e

# Pull the latest version of the repository
echo ""
echo "## Getting latest version from GitHub"
echo ""
git pull origin master

# Compile the server
echo ""
echo "## Compiling tcp_server.cpp"
echo ""
g++ tcp_server.cpp -o tcp_server
echo "log: server compiled as ./tcp_server"

# Start the server on port 7000 (must be set as inbound rule on AWS/Azure)
echo ""
echo "## Starting server on port 7000"
echo ""
./tcp_server 7000
