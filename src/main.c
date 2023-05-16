#include "ft_ping.h"
#include "libft.h"

t_ping ping = {0};
t_rtt rtt = {0};
int sockfd = 0;

int main(int ac, char **av) {
    if (ac < 2)
        exit(0);
    ping = parse(ac, av);        
    sockfd = init_socket();

    signal(SIGINT, handle_signal);

    char msg_intro[1024];
    sprintf(msg_intro, "PING %s (%s) %d bytes of data.\n", ping.host, ping.dest, PACKET_SIZE);
    printf("%s", msg_intro);

    char packet[PACKET_SIZE];
    char buffer[PACKET_SIZE + sizeof(struct ip)];
    char msg_ping[1024];
    struct timeval start_time, end_time;
    while (1) {
        create_packet(packet);

        gettimeofday(&start_time, NULL);
        ssize_t bytes_sent = sendto(sockfd, packet, PACKET_SIZE, 0, ping.addr->ai_addr, ping.addr->ai_addrlen);
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
        gettimeofday(&end_time, NULL);

        struct ip *ip_header = (struct ip *) buffer;
        struct icmphdr *icmp_header = (struct icmphdr *) (buffer + (ip_header->ip_hl << 2)); // Skip IP header
        char *src_addr = NULL;
        int ttl = 0;
        double elapsed_time_ms = 0.0;
        unsigned short id = 0;
        unsigned short sequence = 0;

        ping.transmitted++;
        elapsed_time_ms = get_elapsed_time(start_time, end_time);
        handle_rtt(elapsed_time_ms);

        if (icmp_header->type == ICMP_ECHOREPLY) {        
            unsigned short raw_id = icmp_header->un.echo.id;
            unsigned short raw_seq = icmp_header->un.echo.sequence;

            // Convert from network byte order (big-endian) to host byte order (we could have used ntohs())
            id = ((raw_id >> 8) & 0xff) | ((raw_id & 0xff) << 8);
            sequence = ((raw_seq >> 8) & 0xff) | ((raw_seq & 0xff) << 8);

            src_addr = (char *) inet_ntop(AF_INET, &ip_header->ip_src, buffer, sizeof(buffer));
            ttl = ip_header->ip_ttl;

            ping.received++;

            if (icmp_header->code != 0) {
                printf("Error: ICMP code indicates an error. Code: %d\n", icmp_header->code);
                // Handle the error as needed
            }
        } else {
            printf("Not an ICMP echo reply\n");
            // Handle the error as needed
        }
        sprintf(msg_ping, "%lu bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n", bytes_received - sizeof(struct ip), src_addr, sequence, ttl, elapsed_time_ms);
        printf("%s", msg_ping);
        usleep(1000000);
    }
}