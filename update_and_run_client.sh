#!/bin/bash
set -e

if [ -z "$1" ]; then
   echo "Usage: bash $0 <ip_address_of_server>"
   exit
fi

ADDR=$1
PORT=7000

# Pull the latest version of the repository
echo ""
echo "## Getting latest version from GitHub"
echo ""
git pull origin master

# Compile the client
echo ""
echo "## Compiling src/tcp_client.cpp"
echo ""
g++ src/tcp_client.cpp -o src/tcp_client
echo "log: client compiled as ./src/tcp_client"

# Start the client with address given in argument and port 7000
echo ""
echo "## Connecting to address $ADDR on port $PORT"
echo ""
./src/tcp_client $ADDR:$PORT
