#include "ip_helpers.hpp"
#include "game_parameters.hpp"

uint16_t adjust_to_range(uint32_t input){
   if(input > angle_range/2){
      return (uint16_t)(-(input-50));
   } else {
      return (uint16_t)input;
   }
}

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
        log_verbose("<< %u", tSendStart-tConnectStart, value);

        send_helper(s, &value, 4);

        uint32_t received;
        double tRecvStart=now();
        recv_helper(s, &received, 4);
        double tRecvEnd=now();

        // If receives
        // 0: game is running
        // 1-1000: player number
        // 1000-2000: ranking
        // 1xxxxyyyy-2xxxxyyyy: target coordinates
        if((received >= 1) && (received <= 1000)){
           log_info(">> Hello, your player number is: %u", received);
        } else if((received >= 1001) && (received <= 2000)){
           log_info(">> Congratulations, your ranking is: %u", received);
        } else if((received >= 100000000) && (received <= 200000000)){
           //remove coord_tag
           uint32_t coordinates = received - 100000000;
           //extract Y angle
           uint16_t Y_angle = adjust_to_range(coordinates % 10000);
           //remove Y angle from remaining value and extract X_angle
           coordinates -= Y_angle;
           coordinates /= 10000;
           uint16_t X_angle = adjust_to_range(coordinates);
           log_info(">> Game started: Move your board to the right angle! (%d,%d)", X_angle, Y_angle);
        } else if(received == 0){
           log_info(">> Game running...");
        } else {
           log_verbose(">> unknown message received: %u", received);
        }
        close(s);

        sleep(0.8);
    }
}
