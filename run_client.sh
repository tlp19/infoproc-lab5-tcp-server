#!/bin/bash
set -e

if [ -z "$1" ]; then
   echo usage: $0 directory
   exit
fi

ADDR=$1
PORT=7000

# Start the client with address given in argument and port 7000
echo ""
echo "## Connecting to address $ADDR on port $PORT"
echo ""
./src/tcp_client $(ADDR):$(PORT)
