#!/bin/bash
set -e

# Start the server on port 7000 (must be set as inbound rule on AWS/Azure)
echo ""
echo "## Starting server on port 7000"
echo ""
./src/tcp_server 7000
