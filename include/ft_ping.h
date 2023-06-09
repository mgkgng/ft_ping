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
#define IP_HEADER_SIZE 20

#define FLAG_V (1 << 0)
#define FLAG_H (1 << 1)
#define FLAG_C (1 << 2)
#define FLAG_Q (1 << 3)
#define FLAG_I (1 << 4)
#define FLAG_D (1 << 5)
#define FLAG_T (1 << 6)

/* ---------- */
/* Structures */
/* ---------- */

typedef struct s_ping {
    int flags;
    int check_param;
    int count;
    int ttl;
    float interval;
    char *host;
    char dest[INET_ADDRSTRLEN];
    char name[NI_MAXHOST];
    struct addrinfo *addr;
    int packet_seq;
    int transmitted;
    int received;
} t_ping;

typedef struct s_rtt {
    double min;
    double max;
    double avg;
    double stddev;
} t_rtt;

/* ---------------- */
/* Global variables */
/* ---------------- */

extern t_ping ping;
extern t_rtt rtt;
extern int sockfd;


/* ------------------ */
/* Function prototypes */
/* ------------------ */

/* Ping & Networking */
int init_socket();
void create_packet(char *packet);
void send_packet(char *packet, struct timeval *start_time);
ssize_t receive_packet(char* buffer, struct timeval* end_time);
void process_icmp_reply(char *buf, ssize_t bytes_received, double elapsed_time);

/* Parsing */
t_ping parse(int ac, char **av);
int get_count(char *s);
float get_interval(char *s);
int get_ttl(char *s);

/* Utils */
unsigned short compute_icmp_checksum (unsigned short *paddress, int len);
double get_elapsed_time(struct timeval start_time, struct timeval end_time);
void handle_rtt(double elapsed_time);

/* Printing */
void print_intro();
void print_ping(ssize_t bytes_received, char* src_addr, unsigned short sequence, int ttl, double elapsed_time_ms);
void print_summary();
void print_help();

/* Libft */
int ft_strlen(const char *s);
void ft_strcpy(char *dst, const char *src);
void ft_bzero(void *s, size_t n);
int ft_atoi(const char *str, int strict);
int ft_isfloat(char *str);