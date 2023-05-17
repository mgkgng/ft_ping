#include "ft_ping.h"

t_ping ping = {0};
t_rtt rtt = {0};
int sockfd = 0;

void terminate_success() {
    print_summary();
    close(sockfd);
    freeaddrinfo(ping.addr);
    exit(EXIT_SUCCESS);
}

void handle_signal(int signal) {
    if (signal == SIGINT)
        terminate_success();
}

int main(int ac, char **av) {
    if (ac < 2)
        exit(0);
    ping = parse(ac, av);
    sockfd = init_socket();

    signal(SIGINT, handle_signal);
    print_intro();

    char packet[PACKET_SIZE];
    char buffer[PACKET_SIZE + sizeof(struct ip)];
    struct timeval start_time, end_time;
    while (1 && (ping.count == 0 || ping.transmitted < ping.count)) {
        create_packet(packet);

        send_packet(packet, &start_time);
        ssize_t ret = receive_packet(buffer, &end_time);

        ping.transmitted++;
        double elapsed_time = get_elapsed_time(start_time, end_time);
        handle_rtt(elapsed_time);

        process_icmp_reply(buffer, ret, elapsed_time);

        usleep((ping.flags & FLAG_I) ? ping.interval * 1000000 : 1000000);
    }
    terminate_success();
}