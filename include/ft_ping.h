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
#include <signal.h>

#define PACKET_SIZE 64
#define ICMP_HEADER_SIZE 8

#define FLAG_V (1 << 0)
#define FLAG_H (1 << 1)

/* Structures */

typedef struct s_ping {
    int flags;
    int ttl;
    int packet_size;
    int timeout;
    char *host;
    char *dest;
    struct addrinfo *addr;

    int packet_seq;
    int transmitted;
    int received;
} t_ping;

struct icmphdr { // Create ICMP header
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

typedef struct s_rtt { // Round-trip Time
    double min;
    double max;
    double avg;
    double stddev;
} t_rtt;

/* Global variables */
extern t_ping ping;
extern t_rtt rtt;
extern int sockfd;

/* Functions */
t_ping parse(int ac, char **av);
int init_socket();
unsigned short compute_icmp_checksum (unsigned short *paddress, int len);
void create_packet(char *packet);
double get_elapsed_time(struct timeval start_time, struct timeval end_time);
void handle_signal(int signal);
void handle_rtt(double elapsed_time);