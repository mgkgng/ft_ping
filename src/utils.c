#include "ft_ping.h"

unsigned short compute_icmp_checksum (unsigned short *paddress, int len) {
    int sum = 0;
    unsigned short answer = 0;
    unsigned short *w = paddress;
    int nleft = len;

    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1) {
        *((unsigned char *)&answer) = *((unsigned char *)w);
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}

double get_elapsed_time(struct timeval start_time, struct timeval end_time) {
    return (end_time.tv_sec - start_time.tv_sec) * 1000.0 + (end_time.tv_usec - start_time.tv_usec) / 1000.0;
}

void handle_rtt(double elapsed_time) {
    rtt.min = rtt.min == 0 || elapsed_time < rtt.min ? elapsed_time : rtt.min;
    rtt.max = elapsed_time > rtt.max ? elapsed_time : rtt.max;
    rtt.avg = ping.received == 0 ? elapsed_time : (rtt.avg * (ping.received - 1) + elapsed_time) / ping.received;
    rtt.stddev += (elapsed_time - rtt.avg) * (elapsed_time - rtt.avg);
}