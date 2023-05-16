#include "ft_ping.h"

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

void send_packet(char *packet, struct timeval *start_time) {
    gettimeofday(start_time, NULL);
    ssize_t bytes_sent = sendto(sockfd, packet, PACKET_SIZE, 0, ping.addr->ai_addr, ping.addr->ai_addrlen);
    if (bytes_sent < 0) {
        perror("Packet sending failed");
        exit(EXIT_FAILURE);
    }
}

ssize_t receive_packet(char* buf, struct timeval* end_time) {
    struct sockaddr_in client_addr = {0};
    socklen_t addr_len = sizeof(client_addr);
    
    struct iovec iov[1]; // Array of data buffers
    iov[0].iov_base = buf;
    iov[0].iov_len = sizeof(buf);

    struct msghdr message = {0};
    message.msg_name = &client_addr; // Socket address where the data should be received
    message.msg_namelen = addr_len;
    message.msg_iov = iov;
    message.msg_iovlen = 1; // Number of elements in the array

    ssize_t ret = recvmsg(sockfd, &message, 0);
    if (ret < 0) {
        perror("Response receiving failed");
        exit(EXIT_FAILURE);
    }
    gettimeofday(end_time, NULL);
    return (ret);
}

void process_icmp_reply(char *buf, ssize_t ret, double elapsed_time) {
    struct ip *ip_header = (struct ip *) buf;
    struct icmphdr *icmp_header = (struct icmphdr *) (buf + (ip_header->ip_hl << 2)); // Skip IP header

    if (icmp_header->type == ICMP_ECHOREPLY) {
        ping.received++;
    
        // Convert from network byte order (big-endian) to host byte order (we could have used ntohs())
        unsigned short raw_seq = icmp_header->un.echo.sequence;
        unsigned short sequence = ((raw_seq >> 8) & 0xff) | ((raw_seq & 0xff) << 8);

        char *src_addr = (char *) inet_ntop(AF_INET, &ip_header->ip_src, buf, sizeof(buf));
        printf("%s ", src_addr);
        int ttl = ip_header->ip_ttl;

        if (icmp_header->code != 0)
            printf("ICMP code indicates an error. Code: %d\n", icmp_header->code);

        print_ping(ret - sizeof(struct ip), src_addr, sequence, ttl, elapsed_time);
    } else
        printf("Error: received ICMP type %d\n", icmp_header->type);
}
