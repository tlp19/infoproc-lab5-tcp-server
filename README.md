# InfoProc Coursework - TCP Server and Client in C++

## Installation

### Setting up a AWS/Azure instance to host the Server

Note: if you already have a working AWS/Azure instance, you can skip the first instruction.

1. Instanciate a free instance on your provider of choice following the instructions provided in the lab
2. The server will listen to connections being made on port 7000 (by default). So, in the Security section, you should add a new Inbound Rule for TCP connections being made on port 7000 from anywhere.
3. It is also recommended to save your key pair on your home directory ```~``` as this is the location that the following instructions will use.

### Connecting to the remote instance

You can connect using the command ```
ssh -i ~/<KeyPairToFileName>.pem ubuntu@<IpAddressOfInstance>```

You might have to type "yes" in order to add the ip address to the list of know servers.
You might also have to change the privacy settings of the .pem file using ```$ chmod u-rwx ~/<KeyPairFileName>.pem``` and then try again.

### Installing the server on your instance

1. Check for updates of your packages by running ```$ sudo apt-get update```
2. Install the g++ package using ```$ sudo apt-get install g++```
3. If it is not installed by default, install or update Git using ```$ sudo apt-get install git```
4. Clone the project repository to your remote instance using ```$ git clone https://github.com/inigo-selwood-imperial/de10-game.git```
5. Move into the TCP server directory using ```$ cd de10-game/tcp_server_and_client```

## Running the Game

### Starting the server

To start the server, you just have to run the ```update_and_start_server.sh``` script.
This is done by running the command ```$ bash update_and_start_server.sh```.

_Note: using just ```$ ./update_and_start_server.sh``` might not work as the script requires special permissions._

This script will do the following operations:
1. Pull the latest version of the server from GitHub so that it is always up-to-date.
2. Compile the source code from ```src/tcp_server.cpp``` as ```bin/tcp_server```.
3. Start the server on port 7000 (the default port).

If you have already used the script before, and don't want to pull the whole repository, you can also just directly run the server with ```$ ./src/tcp_server 7000```

### Starting the clients

Once you have started the server, you can tell each player to connect to it with their client program.

As a player, to run the client, you need to have cloned this repository on your local machine, have g++ installed, and run
```$ bash update_client.sh```
to get the latest version of the client program and compile it.

You can then run the client using ```$ ./src/tcp_client <address>:<port>```

Where:
- ```<address>``` is the IPv4 address of the server.
- ```<port>``` is 7000.

### Controlling the game

The terminal will hold twice during the execution of the server. To continue, you will be prompted to press the ```ENTER``` key of your keyboard.


### Closing the server

At the end of the round, both server and clients will stop automatically. Should you need to close either one manually, you can press ```CTRL + C``` on any terminal to stop the specific program.

### Changing game parameters

If you either want to increase the maximum number of player allowed in a single game (```backlog```), or the range that angles are being randomly generated at (```angle_range```), you can modify either of these parameters in ```include/game_parameters.hpp```.
