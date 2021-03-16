#!/bin/bash
set -e

# Pull the latest version of the repository
echo ""
echo "## Getting latest version from GitHub"
echo ""
git pull origin master

# Compile the server
echo ""
echo "## Compiling src/tcp_client.cpp"
echo ""
g++ src/tcp_client.cpp -o src/tcp_client
echo "log: client compiled as ./src/tcp_client"

# Connect to the server adsress and port with client
echo ""
echo "## Connecting to server at address $1 on port 7000"
echo ""
./src/tcp_client $1:7000
