# InfoProc Coursework - TCP Server and Client in C++

&nbsp;

## Informations

This server is designed to be used on Ubuntu 18.04 on a remote instance, either on AWS or Azure. This project was developed and tested on an Amazon EC2 t2.micro instance.

&nbsp;

## Installation

### Setting up a AWS/Azure instance to host the Server

_Note: if you already have a working AWS/Azure instance, you can skip the first instruction._

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
3. If not installed by default, install, or update Git using ```$ sudo apt-get install git```
4. Clone the project repository to your remote instance using ```$ git clone https://github.com/inigo-selwood-imperial/de10-game.git```
5. Move into the TCP server directory using ```$ cd de10-game/tcp_server_and_client```

&nbsp;

## Running the Game

### Starting the server

To compile and start the server, you just have to run the ```update_and_start_server.sh``` script.
This is done by running the command ```$ bash update_and_start_server.sh```.

_Note: using just ```$ ./update_and_start_server.sh``` might not work as the script requires special permissions._

This script will do the following operations:
1. Pull the latest version of the server from GitHub so that it is always up-to-date.
2. Compile the source code from ```src/tcp_server.cpp``` as ```src/tcp_server```.
3. Start the server on port 7000 (the default port).

Latter on, if you want to start the server again without pulling the whole repository from GitHub, use ```$ bash start_server.sh```

### Starting the clients

Once you have started the server, you can tell each player to connect to it with their client program.

_Note: You need to start the server before running the clients. Otherwise, the client program will not have an address to connect to and will terminate immediately.

As a player, to run the client, you need to have cloned this repository, have g++ installed, and run ```$ bash update_and_run_client.sh <address>```, where ```<address>``` is the IPv4 address of the server, to get the latest version of the client program, compile it, and run it.

Latter on, if you want to run the client again without pulling the whole repository from GitHub, use ```$ bash run_client.sh <address>```

### Controlling the game

The terminal will hold twice during the execution of the server. To continue, you will be prompted to press the ```ENTER``` key of your keyboard.

The first time this will happen is right after starting the server: the server will hold before accepting any incoming connections so that every clients have the time to be started and try to connect to the server. After pressing ```ENTER```, the server will cycle through all incoming connections and add them to the player list.

Once this is done, the server terminal will hold again, and this time pressing ```ENTER``` will start the game and make the server do the following operations:
- Send the randomly generated coordinates to every client that are listed as players.
- Listen for the times of the players sent by each client.
- Once every time has been received, rank the players by time, and send each player its rank.


### Closing the server

At the end of the round, both server and clients will stop automatically. Should you need to close either one manually, you can press ```CTRL + C``` on any terminal to stop the specific program.

### Changing game parameters

If you either want to increase the maximum number of player allowed in a single game (```backlog```), or the range that angles are being randomly generated at (```angle_range```), you can modify either of these parameters in ```include/game_parameters.hpp```.

#### Angle generation

Both X-axis and Y-axis coordinates are being generated randomly using the C++ ```<random>``` header.

First, a truly random number generator (```random_device```) is used to generate a seed that is fed to a pseudo random number generator (```mt19936```) that is then used to generate both X and Y axis angles.

