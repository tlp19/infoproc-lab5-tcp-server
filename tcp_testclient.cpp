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
        auto status=connect(s, (const sockaddr*)&dst_addr, sizeof(dst_addr));
        check_status(status!=-1, "Connect failed.");

        double tSendStart=now();
        log_verbose(">> %u", tSendStart-tConnectStart, value);

        send_helper(s, &value, 4);

        uint32_t received;
        double tRecvStart=now();
        recv_helper(s, &received, 4);
        double tRecvEnd=now();

        // If receives
        // 0: game is running
        // 1-1000: player number
        // 1000-2000: ranking
        // 1xxxyyyzzz-2xxxyyyzzz: target coordinates
        if((received >= 1) && (received <= 1000)){
           log_info("[Received] Hello, your player number is: %u", received);
        } else if((received >= 1001) && (received <= 2000)){
           log_info("[Received] Congratulations, your ranking is: %u", received);
        } else if((received >= 1000000000) && (received <= 2000000000)){
           log_info("[Received] Game started: Move your board to the right angle!");
        } else if(received == 0){
           log_info("[Received] Game running...");
        }
        close(s);

        sleep(1);
    }
}
