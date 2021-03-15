#include "ip_helpers.hpp"

int main(int argc, char *argv[])
{
    log_init();

    sockaddr_in dst_addr=make_sockaddr_in(argv[1]);

    while(1){
        int s=socket(AF_INET, SOCK_STREAM, 0);
        check_status(s!=-1, "Couldn't create socket", errno);

        uint32_t value=0;

        double tConnectStart=now();
        if(log_verbose_enabled()){
            log_stream_verbose()<<"Connecting to server at address "<<sockaddr_in_to_string(dst_addr)<<"\n";
        }
        auto status=connect(s, (const sockaddr*)&dst_addr, sizeof(dst_addr));
        check_status(status!=-1, "Connect failed.");

        double tSendStart=now();
        log_verbose("Connected, sending update %u to server", tSendStart-tConnectStart, value);

        send_helper(s, &value, 4);

        uint32_t payload;
        double tRecvStart=now();
        recv_helper(s, &payload, 4);
        double tRecvEnd=now();
        log_info("Received, tConnect=%.8f, tSend=%.8f, tRecv=%.8f, tTotal=%.8f; accumulator %u", tSendStart-tConnectStart, tRecvStart-tSendStart, tRecvEnd-tSendStart, tRecvEnd-tConnectStart, payload);

        log_verbose("Closing connection.");
        close(s);

        sleep(1);
    }
}
