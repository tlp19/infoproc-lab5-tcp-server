#!/bin/bash
set -e

# Pull the latest version of the repository
echo ""
echo "## Getting latest version from GitHub"
echo ""
git pull origin master

# Compile the server
echo ""
echo "## Compiling tcp_testserver.cpp"
echo ""
g++ tcp_testserver.cpp -o tcp_testserver
echo "log: server compiled as ./tcp_testserver"

# Start the server on port 7000 (must be set as inbound rule on AWS/Azure)
echo ""
echo "## Starting test server on port 7000"
echo ""
./tcp_testserver 7000
