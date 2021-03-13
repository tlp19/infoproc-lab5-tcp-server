#include "ip_helpers.hpp"

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
    int backlog=16;  // Number of pending connections before clients are refused
    status=listen(s, backlog);
    check_status(status!=-1, "Couldn't listen on socket.");

    uint32_t accumulator=0;

    while(1){
       log_verbose("Waiting to accept");

        sockaddr_in src_addr;
        socklen_t src_addr_len=sizeof(sockaddr_in);

        int client=accept(s, (sockaddr*)&src_addr, &src_addr_len);
        check_status(client!=-1, "accept failed.", errno);

        if(log_verbose_enabled()){
            log_stream_verbose()<<"Established connection with client addr="<<sockaddr_in_to_string(src_addr)<<"\n";
        }

        uint32_t payload;
        log_verbose("Waiting to receive");
        recv_helper(client,
            &payload, 4
        );

        accumulator += payload;

        if(log_info_enabled()){
            log_stream_info()<<"Received "<<payload<<" from addr="<<src_addr.sin_addr.s_addr<<":"<<src_addr.sin_port<<"; new accumulator = "<<accumulator<<"\n";
        }

        send_helper(client,
            &accumulator, 4
        );

        log_verbose("Closing channel.");
        close(client);
    }
}