#include "ft_ping.h"

void print_intro() {
    sprintf(msg.intro, "PING %s (%s) %d bytes of data.\n", ping.host, ping.dest, PACKET_SIZE);
    printf("%s", msg.intro);
}

void print_ping(ssize_t ret, char *src_addr, unsigned short sequence, int ttl, double elapsed_time) {
    ft_bzero(msg.ping, MSG_BUFFER);
    sprintf(msg.ping, "%lu bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", ret - sizeof(struct ip), src_addr, sequence, ttl, elapsed_time);
    printf("%s", msg.ping);
}

void print_summary() {
    sprintf(msg.summary, "\n--- %s ping statistics ---\n"
        "%d packets transmitted, %d packets received, %.1f%% packet loss\n"
        "round-trip min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
        ping.host, ping.transmitted, ping.received,
        (float)(ping.transmitted - ping.received) / ping.transmitted * 100,
        rtt.min, rtt.avg, rtt.max, rtt.stddev);
    printf("%s", msg.summary);
}