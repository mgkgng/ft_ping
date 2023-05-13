#include "ft_ping.h"
#include "libft.h"

int g_id = 0;

void exit_error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int ac, char **av) {
    if (ac < 2)
        exit(0);
    t_ping *ping = parse(ac, av);
    if (!ping)
        exit(EXIT_FAILURE);
    int sockfd = init_socket();

    char packet[PACKET_SIZE];
    char buffer[PACKET_SIZE + sizeof(struct ip)];
    while (1) {
        create_packet(packet);

        ssize_t bytes_sent = sendto(sockfd, packet, PACKET_SIZE, 0, ping->addr->ai_addr, ping->addr->ai_addrlen);
        if (bytes_sent < 0) {
            perror("Packet sending failed");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in client_addr = {0};
        socklen_t addr_len = sizeof(client_addr);

        struct iovec iov[1]; // Array of data buffers
        iov[0].iov_base = buffer;
        iov[0].iov_len = sizeof(buffer);

        struct msghdr message = {0};
        message.msg_name = &client_addr; // Socket address where the data should be received
        message.msg_namelen = addr_len;
        message.msg_iov = iov;
        message.msg_iovlen = 1; // Number of elements in the array

        ssize_t bytes_received = recvmsg(sockfd, &message, 0);
        if (bytes_received < 0) {
            perror("Response receiving failed");
            exit(EXIT_FAILURE);
        }

        struct ip *ip_header = (struct ip *) buffer;
        struct icmphdr *icmp_header = (struct icmphdr *) (buffer + (ip_header->ip_hl << 2)); // Skip IP header

        if (icmp_header->type == ICMP_ECHOREPLY) {
            printf("ICMP echo reply received\n");
        
            unsigned short raw_id = icmp_header->un.echo.id;
            unsigned short raw_seq = icmp_header->un.echo.sequence;

            // Convert from network byte order (big-endian) to host byte order (we could have used ntohs())
            unsigned short id = ((raw_id >> 8) & 0xff) | ((raw_id & 0xff) << 8);
            unsigned short sequence = ((raw_seq >> 8) & 0xff) | ((raw_seq & 0xff) << 8);

            printf("ID: %d\n", id);
            printf("Sequence: %d\n", sequence);

            printf("Source IP: %s\n", inet_ntop(AF_INET, &ip_header->ip_src, buffer, sizeof(buffer)));
            printf("Time-To-Live: %d\n", ip_header->ip_ttl);

        }
        usleep(1000000);
    }
    close(sockfd);
    freeaddrinfo(ping->addr);
    free(ping);
}