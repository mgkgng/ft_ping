#include "ft_ping.h"
#include "libft.h"

int init_socket() {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        fprintf(stderr, "ft_ping: socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // Set socket options to enable ICMP protocol
    const int ttl = 255;
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        fprintf(stderr, "ft_ping: setsockopt: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return (sockfd);
}

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

void create_packet(char *packet) {
    ft_bzero(packet, PACKET_SIZE);

    // Apparently, the IP header is not needed when sending ICMP packets if IPPROTO_ICMP is used as protocol when creating the socket
    struct icmphdr *icmp_header = (struct icmphdr *) packet;
    icmp_header->type = ICMP_ECHO;
    icmp_header->code = 0;
    icmp_header->un.echo.id = htons(getpid() & 0xFFFF);
    icmp_header->un.echo.sequence = htons(ping.packet_seq++);
    icmp_header->checksum = 0;
    icmp_header->checksum = compute_icmp_checksum((unsigned short *) icmp_header, sizeof(struct icmphdr));
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