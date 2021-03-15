#ifndef ip_helpers_hpp
#define ip_helpers_hpp

// OS-specific headers
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>

// C headers exposed to C++
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cassert>

// C++ headers
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

// My headers
#include <stdlib.h>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <random>

double now()
{
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec+1e-9*ts.tv_nsec;
}

// Process-wide log-level
int g_log_level=4; // default is verbose

void log_init()
{
    auto p=getenv("IP_LOG_LEVEL");
    if(p){
        g_log_level=atoi(p);
        fprintf(stderr, "Log level = %d\n", g_log_level);
    }
}

bool log_verbose_enabled()
{ return g_log_level>=4; }

bool log_info_enabled()
{ return g_log_level>=3; }

bool log_warn_enabled()
{ return g_log_level>=2; }

bool log_error_enabled()
{ return g_log_level>=1; }

inline void log_impl(const char *type, const char *msg, va_list args)
{
    double t=now();
    char buffer[256]={0};
    vsnprintf(buffer, sizeof(buffer)-1, msg, args);

    fprintf(stderr, "%.8f (%s) : %s\n", t, type, buffer);
}


inline void log_error(const char *msg, ...)
{
    if(g_log_level>0){
        va_list args;
        va_start(args, msg);
        log_impl("ERROR  ", msg, args);
        va_end(args);
    }
}

inline void log_warn(const char *msg, ...)
{
    if(g_log_level>1){
        va_list args;
        va_start(args, msg);
        log_impl("WARN   ", msg, args);
        va_end(args);
    }
}

inline void log_info(const char *msg, ...)
{
    if(g_log_level>2){
        va_list args;
        va_start(args, msg);
        log_impl("info   ", msg, args);
        va_end(args);
    }
}

inline void log_verbose(const char *msg, ...)
{
    if(g_log_level>3){
        va_list args;
        va_start(args, msg);
        log_impl("verbose", msg, args);
        va_end(args);
    }
}

std::ostream &log_stream(const char *type)
{
    double t=now();
    char buffer[64]={0};
    snprintf(buffer, sizeof(buffer)-1, "%.8f (%s) : ", t, type);
    std::cerr<<buffer;
    return std::cerr;
}

std::ostream &log_stream_verbose()
{ return log_stream("verbose"); }

std::ostream &log_stream_info()
{ return log_stream("info"); }

inline void check_status(bool success, const char *msg, int err=0)
{
    if(!success){
        if(err!=0){
            fprintf(stderr, "Error : %s. errno=%d (%s)\n", msg, err, strerror(err));
        }else{
            fprintf(stderr, "Error : %s\n", msg);
        }
        exit(1);
    }
}

inline sockaddr_in make_sockaddr_in(uint16_t port, uint32_t ip)
{
    sockaddr_in res;
    res.sin_family=AF_INET;
    res.sin_port=htons(port);
    res.sin_addr.s_addr=htonl(ip);
    return res;
}

inline sockaddr_in make_sockaddr_in(uint16_t port, uint8_t ip3, uint8_t ip2, uint8_t ip1, uint8_t ip0)
{
    uint32_t ip = (uint32_t(ip3)<<24)|(uint32_t(ip2)<<16)|(uint32_t(ip1)<<8)|(ip0);
    return make_sockaddr_in(port,ip);
}

/* Turn a string of the form "a.b.c.d:port" into an IPv4 address. */
inline sockaddr_in make_sockaddr_in(const std::string &addr)
{
    int colon=addr.find(':');
    check_status(colon!=std::string::npos, "Missing colon in address ");

    std::string addr_ip=addr.substr(0, colon);
    std::string addr_port=addr.substr(colon+1, std::string::npos);

    unsigned port=std::stoi(addr_port);

    uint32_t ip=0;
    size_t part_start=0;
    unsigned part_count=0;
    do{
        size_t dot=addr_ip.find('.', part_start);
        std::string addr_ip_part=addr_ip.substr(part_start, dot);
        uint32_t part_num=std::stoi(addr_ip_part);
        check_status(part_num<256, "Out of range ip component in address");
        ip=(ip<<8) | part_num;
        part_start = (dot!=std::string::npos) ? dot+1 : dot;
        part_count++;
    }while(part_start!=std::string::npos);
    check_status(part_count==4, "Did not get exactly four parts from address");

    return make_sockaddr_in(port, ip);
}


inline std::string sockaddr_in_to_string(const sockaddr_in &addr)
{
    uint32_t octets=ntohl(addr.sin_addr.s_addr);
    std::stringstream tmp;
    for(int i=0;i<4;i++){
        if(i!=0){
            tmp<<".";
        }
        tmp<<(octets>>24);
        octets=octets<<8;
    }
    tmp<<":"<<ntohs(addr.sin_port);
    return tmp.str();
}

inline void recv_helper(int sockfd, void *buf, size_t len)
{
    char *buf_curr=(char *)buf;
    size_t buf_len=len;
    while(buf_len > 0){
        auto status=recv(sockfd, buf_curr, buf_len, 0);
        check_status(status!=0, "Peer connection was closed.");
        check_status(status>0, "Failure during recv.", errno);

        buf_len -= status;
        buf_curr += status;
    }
}

inline void send_helper(int sockfd, const void *buf, size_t len)
{
    const char *buf_curr=(const char *)buf;
    size_t buf_len=len;
    while(buf_len > 0){
        auto status=send(sockfd, buf_curr, buf_len, 0);
        check_status(status!=0, "Peer connection was closed.");
        check_status(status>0 , "Failure during send.", errno);
        buf_len -= status;
        buf_curr += status;
    }
}

inline void send_all_as_string_helper(int sockfd, const std::string &s)
{
    if(!s.empty()){
        send_helper(sockfd, &s[0], s.size());
    }
}

struct http_line_buffer
{
    const int READ_CHUNK_SIZE=4096;

    std::string buffer;

    std::string read_line(int fd)
    {
        while(1){
            auto eol=buffer.find('\n');
            if(eol!=std::string::npos){
                std::string res=buffer.substr(0, eol+1);
                buffer.erase(buffer.begin(), buffer.begin()+eol+1);
                return res;
            }

            auto valid_size=buffer.size();
            buffer.resize(valid_size+READ_CHUNK_SIZE);

            auto status=recv(fd, &buffer[valid_size], buffer.size()-valid_size, 0);
            check_status(status>0, "Failure during send.", errno);

            buffer.resize(valid_size+status);
        }
    }
};

#endif
