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

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Error setting socket options for timeout");
        exit(EXIT_FAILURE);
    }

    if (ping.flags & FLAG_D) {
        int debug_option = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_DEBUG, &debug_option, sizeof(debug_option)) < 0) {
            perror("Error setting socket options for debug mode");
            exit(EXIT_FAILURE);
        }
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
    }
}

ssize_t receive_packet(char *buffer, struct timeval *end_time) {
    struct sockaddr_in client_addr = {0};
    socklen_t addr_len = sizeof(client_addr);
    
    struct iovec iov[1]; // Array of data buffers
    iov[0].iov_base = buffer;
    iov[0].iov_len = PACKET_SIZE + sizeof(struct ip);

    struct msghdr message = {0};
    message.msg_name = &client_addr; // Socket address where the data should be received
    message.msg_namelen = addr_len;
    message.msg_iov = iov;
    message.msg_iovlen = 1; // Number of elements in the array

    ssize_t ret = recvmsg(sockfd, &message, 0);
    gettimeofday(end_time, NULL);
    return (ret);
}

void handle_icmp_error(struct icmphdr *icmp_header) {
    printf("Request timeout for icmp_seq %d\n", ping.transmitted - 1);

    switch(icmp_header->type) {
        case ICMP_TIME_EXCEEDED:
            printf("Error: Time to live exceeded\n");
            break;
        case ICMP_UNREACH:
            printf("Error: Destination Unreachable\n");
            break;
        case ICMP_SOURCEQUENCH:
            printf("Error: Source Quench\n");
            break;
        case ICMP_REDIRECT:
            printf("Error: Redirect Message\n");
            break;
        case ICMP_PARAMPROB:
            printf("Error: Parameter Problem\n");
            break;
        default:
            printf("Error: Unknown ICMP message type: %d\n", icmp_header->type);
    }
}

void process_icmp_reply(char *buffer, ssize_t ret, double elapsed_time) {
    struct ip *ip_header = (struct ip *) buffer;
    struct icmphdr *icmp_header = (struct icmphdr *) (buffer + (ip_header->ip_hl << 2)); // Skip IP header

    if (icmp_header->type == ICMP_ECHOREPLY) {
        ping.received++;

        // Convert from network byte order (big-endian) to host byte order (we could have used ntohs())
        unsigned short raw_seq = icmp_header->un.echo.sequence;
        unsigned short sequence = ((raw_seq >> 8) & 0xff) | ((raw_seq & 0xff) << 8);

        char *src_addr = (char *) inet_ntop(AF_INET, &ip_header->ip_src, buffer, PACKET_SIZE + sizeof(struct ip));
        int ttl = ip_header->ip_ttl;

        if (icmp_header->code != 0)
            printf("ICMP code indicates an error. Code: %d\n", icmp_header->code);

        if (!(ping.flags & FLAG_Q))
            print_ping(ret, src_addr, sequence, ttl, elapsed_time);
    } else
        handle_icmp_error(icmp_header);
}
