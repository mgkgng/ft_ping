#pragma once

#include <sys/time.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>

#define PACKET_SIZE 64
#define ICMP_HEADER_SIZE 8

#define FLAG_V (1 << 0)
#define FLAG_H (1 << 1)

typedef struct s_ping {
    int flags;
    int ttl;
    int packet_size;
    int count;
    int interval;
    int timeout;
    char *host;
    struct addrinfo *addr;
} t_ping;

// Create ICMP header
struct icmphdr {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    union {
        struct {
            uint16_t id;
            uint16_t sequence;
        } echo;
        uint32_t gateway;
        struct {
            uint16_t mtu;
        } frag;
    } un;
};
