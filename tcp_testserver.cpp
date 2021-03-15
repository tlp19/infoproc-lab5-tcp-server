#include "ip_helpers.hpp"
#include "game_parameters.hpp"


int main(int argc, char *argv[])
{
    log_init();

    int port=atoi(argv[1]);

    int s=socket(AF_INET, SOCK_STREAM, 0);
    check_status(s!=-1, "Couldn't create socket", errno);

    // Make it possible to re-open socket immediately if you kill previous instance
    int enable=1;
    int status=setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    check_status(status!=-1, "Couldnt set SO_REUSEADDR", errno);

    sockaddr_in local_addr=make_sockaddr_in(port, 0 );

    log_info("Binding to address %s", sockaddr_in_to_string(local_addr).c_str());
    status=bind(s, (const sockaddr*)&local_addr, sizeof(local_addr));
    check_status(status!=-1, "Couldn't bind socket");

    log_info("Listening for incoming connections");
    status=listen(s, Parameters::backlog);
    check_status(status!=-1, "Couldn't listen on socket.");


    // Pause
    std::cout << std::endl << "Tell all clients to connect to the server (up to " <<Parameters::backlog<< "), then press ENTER:";
    std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );




    // Store all clients that have connected once
    std::unordered_map<uint32_t, uint32_t> connected_players;
               //ip_address   //nb_players
    uint32_t nb_players = 0;

    //List all clients trying to access server
    for(int i = 1; i <= Parameters::backlog ; i++){
      // Establish connection with one client at a time
      sockaddr_in src_addr;
      socklen_t src_addr_len=sizeof(sockaddr_in);
      int client=accept(s, (sockaddr*)&src_addr, &src_addr_len);
      check_status(client!=-1, "accept failed.", errno);
      if(log_verbose_enabled()){
           log_stream_verbose()<<"Established connection with client addr="<<sockaddr_in_to_string(src_addr)<<"\n";
      }
      // Check if client was connected before or not
      if(connected_players.find(src_addr.sin_addr.s_addr) == connected_players.end()){
         //Client not connected before:

         // Receive one packet
         uint32_t received;
         recv_helper(client,
              &received, 4
         );

         // Increment the number of player by one
         nb_players++;
         // Add the address to the list of clients connected
         connected_players[src_addr.sin_addr.s_addr] = nb_players;
         // Send back to the client the new player number
         send_helper(client,
              &nb_players, 4
         );
      } else {
         //Previous client:

         // Receive one packet
         uint32_t received;
         recv_helper(client,
              &received, 4
         );

         // Send back to the client the previously assigned player number
         send_helper(client,
              &connected_players[src_addr.sin_addr.s_addr], 4
         );
      }
      close(client);
    }



    // Pause
    std::cout << std::endl << "Found " << nb_players << " players." << std::endl;
    std::cout << std::endl << "Press ENTER to send coordinates and start game:";
    std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );


    std::unordered_set<uint32_t> started_players;
                     //ip_address

    uint32_t send_default = 0;

    // Generate random coordinates
    std::random_device seed; //truly random seed (slow)
    std::mt19937 mt(seed()); //pseudo random generator (fast and repeatable)
    std::uniform_int_distribution<uint16_t> dist(0, Parameters::angle_range); //range is 0000 to 9999
    uint16_t random_X_angle = dist(mt);
    uint16_t random_Y_angle = dist(mt);
    // Build the coordinates value to be transmitted (in decimal: 1XXXXYYYY)
    uint32_t coord_tag = 100000000;
    uint16_t X_formatoffset = 10000;
    uint32_t coordinates = coord_tag + X_formatoffset*random_X_angle + random_Y_angle;

    // Send coordinates to all clients in queue until all players have received it
    while(started_players.size() != nb_players){
      // Connect to one of the players at a time
      sockaddr_in src_addr;
      socklen_t src_addr_len=sizeof(sockaddr_in);
      int client=accept(s, (sockaddr*)&src_addr, &src_addr_len);
      check_status(client!=-1, "accept failed.", errno);
      if(connected_players.find(src_addr.sin_addr.s_addr) == connected_players.end()){
         //Client not connected before:
         if(log_verbose_enabled()){
            log_stream_verbose()<<"Unknown client, skipping.\n";
         }
         // Receive one packet and send it back
         uint32_t received;
         recv_helper(client,
              &received, 4
         );
         send_helper(client,
              &received, 4
         );
      } else {
         // Connected player: check if it got the coordinates before
         if(started_players.find(src_addr.sin_addr.s_addr) == started_players.end()){
            // It didn't
            if(log_verbose_enabled()){
               log_stream_verbose()<<"Sending coordinates to player "<<connected_players[src_addr.sin_addr.s_addr]<<"\n";
            }
            started_players.insert(src_addr.sin_addr.s_addr);
            uint32_t received;
            recv_helper(client,
               &received, 4
            );
            // Send the coordinates
            send_helper(client,
               &coordinates, 4
            );
         } else {
            // It did
            if(log_verbose_enabled()){
               log_stream_verbose()<<"Player "<<connected_players[src_addr.sin_addr.s_addr]<<" already had the coordinates\n";
            }
            uint32_t received;
            recv_helper(client,
               &received, 4
            );
            // Send default message
            send_helper(client,
               &send_default, 4
            );

         }
      }
      close(client);
   }

   std::cout << std::endl << "All players got the coordinates, now waiting for all their times..." << std::endl << std::endl;

    std::unordered_map<uint32_t, uint32_t> player_times;

    uint32_t send = 0 ;

    // Check that the players have finished or not
    while(player_times.size() != nb_players){
        // Connect to one of the clients at a time
        sockaddr_in src_addr;
        socklen_t src_addr_len=sizeof(sockaddr_in);
        int client=accept(s, (sockaddr*)&src_addr, &src_addr_len);
        check_status(client!=-1, "accept failed.", errno);
        if(log_verbose_enabled()){
            log_stream_verbose()<<"Established connection with client addr="<<sockaddr_in_to_string(src_addr)<<"\n";
        }
        // Receive data from client
        uint32_t received;
        recv_helper(client,
            &received, 4
        );
        if(log_info_enabled()){
            log_stream_info()<<"Received: "<<received<<"\n";
        }
        // If received data is 0, the player hasn't finshed, other it's the time that it took him to finish
        if(received != 0) {
           player_times[src_addr.sin_addr.s_addr] = received;
        }
        send_helper(client,
            &send, 4
        );
        close(client);
    }




   // Sort ip addresses by time
   std::map<uint32_t, uint32_t> ordered_times(player_times.begin(), player_times.end());
   std::multimap<int, int> sorted_times;
   for(auto const &entry : ordered_times) {
      sorted_times.insert(std::make_pair(entry.second, entry.first));
   }
   // Rank the addresses
   std::vector<uint32_t> ranked_addresses;
   for(auto const &mm_entry : sorted_times) {
      ranked_addresses.push_back(mm_entry.second);
   }

   // Connect to each player and send them their ranking
   std::unordered_set<uint32_t> players_told_rank;

   while(players_told_rank.size() != nb_players){
      // Connect to one of the clients at a time
      sockaddr_in src_addr;
      socklen_t src_addr_len=sizeof(sockaddr_in);
      int client=accept(s, (sockaddr*)&src_addr, &src_addr_len);
      check_status(client!=-1, "accept failed.", errno);
      if(log_verbose_enabled()){
           log_stream_verbose()<<"Established connection with client addr="<<sockaddr_in_to_string(src_addr)<<"\n";
      }
      // Receive data from client
      uint32_t received;
      recv_helper(client,
           &received, 4
      );
      // Get the rank of the player by it's address
      int player_rank;
      for (int i = 0 ; i < ranked_addresses.size() ; i++){
         if(src_addr.sin_addr.s_addr==ranked_addresses[i]){
            player_rank = i+1;
         }
      }
      int send_rank = 1000 + player_rank;
      // Send the rank to the player
      if(players_told_rank.find(src_addr.sin_addr.s_addr) == players_told_rank.end()){
         // Player has never been told his rank
         send_helper(client,
              &send_rank, 4
         );
         // Add the player to the list of players that have been told their rank
         players_told_rank.insert(src_addr.sin_addr.s_addr);
      } else {
         // Player already has been told his rank (shouldn't happen)
         send_helper(client,
              &send_default, 4
         );
      }
      close(client);
   }

}
