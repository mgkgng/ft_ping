#include "ft_ping.h"
#include "libft.h"
#include <pcap.h>

t_ping *g_ping_info;
int g_sockfd;
int g_id = 0;
int g_seq = 0;

char g_local_ip[INET_ADDRSTRLEN] = {0}; 
char g_dest_ip[INET_ADDRSTRLEN] = {0};

void exit_error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void terminate() {
    close(g_sockfd);
    freeaddrinfo(g_ping_info->addr);
    free(g_ping_info);
}

int main(int ac, char **av) {
    if (ac < 2)
        exit(0);
    t_ping *ping = parse(ac, av);
    g_sockfd = init_socket();

    char packet[PACKET_SIZE];
    char buffer[PACKET_SIZE];
    while (1) {
        create_packet(packet);

        printf("sending\n");
        ssize_t bytes_sent = sendto(g_sockfd, packet, PACKET_SIZE, 0, (struct sockaddr *) &ping->addr, sizeof(ping->addr));
        if (bytes_sent < 0) {
            perror("Packet sending failed");
            terminate();
            exit(EXIT_FAILURE);
        }
        printf("sent\n");

        struct sockaddr_in client_addr = {0};
        socklen_t addr_len = sizeof(client_addr);
        printf("receiving\n");
        ssize_t bytes_received = recvfrom(g_sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr *) &client_addr, &addr_len);
        printf("received\n");
        if (bytes_received < 0) {
            perror("Response receiving failed");
            terminate();
            exit(EXIT_FAILURE);
        }
        printf("recevied!! %s\n", buffer);

        printf("ding dong\n");
        usleep(1000000);
    }


    // Close the socket


    terminate();
}