#include <ft_ping.h>
#include <libft.h>

void handle_signal(int signal) {
    if (signal == SIGINT) {
        char msg_summary[1024];
        sprintf(msg_summary, "\n--- %s ping statistics ---\n"
            "%d packets transmitted, %d packets received, %.1f%% packet loss\n"
            "round-trip min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
            ping.host, ping.transmitted, ping.received,
            (float)(ping.transmitted - ping.received) / ping.transmitted * 100,
            rtt.min, rtt.avg, rtt.max, rtt.stddev);
        printf("%s", msg_summary);

        close(sockfd);
        freeaddrinfo(ping.addr);
        free(ping.dest);
        exit(EXIT_SUCCESS);
    }
}