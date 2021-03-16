#!/bin/bash
set -e

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
