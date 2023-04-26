#include "ft_ping.h"
#include "libft.h"

t_ping *g_ping_info;

int get_options(char *flag_str) {
    int res = 0;

    for (int i = 0; flag_str[i]; i++) {
        if (flag_str[i] == 'v')
            res |= FLAG_V;
        else if (flag_str[i] == 'h')
            res |= FLAG_H;
        else
            exit(0); //TODO error msg (invalid flag)
    }
    return (res);
}

t_ping* parse(int ac, char **av) {
    t_ping* res = ft_calloc(1, sizeof(t_ping));

    for (int i = 1; i < ac; i++) {
        if (av[i][0] == '-') {
            if (!av[i][1])
                exit(0); // TODO error msg (invalid flag)
            res->flags |= get_options(av[i] + 1);
        } else {
            if (res->host)
                exit(0); //TODO error msg
            res->host = av[i];
        }
    }
    return (res);
}

void get_address() {
    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int ret = getaddrinfo(g_ping_info->host, NULL, &hints, &g_ping_info->addr);
    if (ret != 0) {
        fprintf(stderr, "ft_ping: cannot resolve %s: Unknown host\n", g_ping_info->host);
        exit(EXIT_FAILURE);
    }
}

void terminate() {
    freeaddrinfo(g_ping_info->addr);
    free(g_ping_info);
}

int main(int ac, char **av) {
    if (ac < 2)
        exit(0);
    g_ping_info = parse(ac, av);    
    get_address();

    // int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    // if (sockfd < 0) {
    //     perror("ft_ping: socket");
    //     exit(EXIT_FAILURE);
    // }

    if (g_ping_info->addr->ai_family != AF_INET) {
        fprintf(stderr, "Only IPv4 accepted\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in *ipv4 = (struct sockaddr_in *) g_ping_info->addr->ai_addr;

    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipv4->sin_addr, ipstr, INET_ADDRSTRLEN);
    printf("IP address: %s\n", ipstr);
    inet_pton(AF_INET, ipstr, &ipv4->sin_addr);
    printf("IP address: %s\n", ipstr);

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd == -1) {
        perror("socket");
        return EXIT_FAILURE;
    }

    // Set socket options to enable ICMP protocol
    const int on = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) == -1) {
        perror("setsockopt");
        return EXIT_FAILURE;
    }


    terminate();
}