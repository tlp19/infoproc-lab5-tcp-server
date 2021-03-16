#!/bin/bash
set -e

# Pull the latest version of the repository
echo ""
echo "## Getting latest version from GitHub"
echo ""
git pull origin master

# Compile the server
echo ""
echo "## Compiling src/tcp_server.cpp"
echo ""
g++ src/tcp_server.cpp -o bin/tcp_server
echo "log: server compiled as ./bin/tcp_server"

# Start the server on port 7000 (must be set as inbound rule on AWS/Azure)
echo ""
echo "## Starting server on port 7000"
echo ""
./bin/tcp_server 7000
