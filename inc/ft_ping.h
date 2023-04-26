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

#define PACKET_SIZE 64
#define ICMP_ECHO_REQUEST 8

#define FLAG_V (1 << 0)
#define FLAG_H (1 << 1)

// declaring a structure
// this is a struct that contains all the options
// that we need to store
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