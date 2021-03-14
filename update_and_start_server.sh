#!bin/bash
set -e

# Pull the latest version of the repository
git pull origin master

# Compile the server
g++ tcp_server.cpp -o tcp_server

# Start the server on port 7000 (must be set as inbound rule on AWS/Azure)
./tcp_server 7000
