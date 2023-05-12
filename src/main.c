#include "ft_ping.h"
#include "libft.h"

int g_id = 0;
int g_seq = 0;

void exit_error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int ac, char **av) {
    if (ac < 2)
        exit(0);
    t_ping *ping = parse(ac, av);
    int sockfd = init_socket();

    char packet[PACKET_SIZE];
    char buffer[PACKET_SIZE];
    while (1) {
        create_packet(packet);

        ssize_t bytes_sent = sendto(sockfd, packet, PACKET_SIZE, 0, ping->addr->ai_addr, ping->addr->ai_addrlen);
        if (bytes_sent < 0) {
            perror("Packet sending failed");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_in client_addr = {0};
        socklen_t addr_len = sizeof(client_addr);
        ssize_t bytes_received = recvfrom(sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr *) &client_addr, &addr_len);
        if (bytes_received < 0) {
            perror("Response receiving failed");
            exit(EXIT_FAILURE);
        }
        printf("recevied!! %s\n", buffer);

        usleep(1000000);
    }
    close(sockfd);
    freeaddrinfo(ping->addr);
    free(ping);
}