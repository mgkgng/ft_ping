#include "ft_ping.h"

void print_intro() {
    printf("PING %s (%s) %d(%d) bytes of data.\n", ping.host, ping.dest, PACKET_SIZE - ICMP_HEADER_SIZE, PACKET_SIZE + IP_HEADER_SIZE);
}

void print_ping(ssize_t ret, char *src_addr, unsigned short sequence, int ttl, double elapsed_time) {
    printf("%lu bytes from %s (%s): icmp_seq=%d ttl=%d time=%.3f ms\n", ret - sizeof(struct ip), ping.name, src_addr, sequence, ttl, elapsed_time);
}

void print_summary() {
    printf("\n--- %s ping statistics ---\n"
        "%d packets transmitted, %d packets received, %.1f%% packet loss\n"
        "round-trip min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
        ping.host, ping.transmitted, ping.received,
        (float)(ping.transmitted - ping.received) / ping.transmitted * 100,
        rtt.min, rtt.avg, rtt.max, rtt.stddev);
}

void print_help() {
    printf("\r\nDescription: \
ft_ping is a network utility that sends ICMP Echo Request packets to a specified destination and awaits ICMP Echo Reply packets in response. \
It tests IP-level connectivity and packet delivery, ensuring successful communication.\n\n\
Use the command to verify connectivity and measure response times.\n \
\n\
Usage: \
ping <destination> [-h] [-v] [-d] [-q] [-i <interval>] [-t <ttl>] [-c <count>]\n\
\n\
Options:\n\
    -h: Display help message\n\
    -v: Enable verbose mode\n\
    -d: Enable debug mode (SO_DEBUG)\n\
    -q: Enable quiet mode\n\
    -i <interval>: Set ping interval\n\
    -t <ttl>: Set TTL\n\
    -c <count>: Set count\n");
}