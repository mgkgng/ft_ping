#include "ft_ping.h"

static int get_options(char *flag_str) {
    int res = 0;

    for (int i = 0; flag_str[i]; i++) {
        if (flag_str[i] == 'v')
            res |= FLAG_V;
        else if (flag_str[i] == 'h')
            res |= FLAG_H;
        else {
            fprintf(stderr, "Error: Invalid flag\n");
            exit(EXIT_FAILURE);
        }
    }
    return (res);
}

static struct addrinfo *get_addr_info(char *host) {
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;

    int ret = getaddrinfo(host, NULL, &hints, &res);
    if (ret != 0) {
        fprintf(stderr, "ft_ping: cannot resolve %s: Unknown host\n", host);
        exit(EXIT_FAILURE);
    } else if (res->ai_family != AF_INET) {
        fprintf(stderr, "Not a valid IPv4 address\n");
        freeaddrinfo(res);
        exit(EXIT_FAILURE);
    }

    return (res);
}

// static char *get_dest(struct addrinfo *addr) {
//     struct sockaddr_in *ipv4 = (struct sockaddr_in *) addr->ai_addr;
//     char ipstr[INET_ADDRSTRLEN];
//     char *res = ft_calloc(1, INET_ADDRSTRLEN);

//     inet_ntop(AF_INET, &ipv4->sin_addr, ipstr, INET_ADDRSTRLEN);
//     ft_strcpy(res, ipstr);
//     printf("dest address: %s\n", res);
//     return (res);
// }

t_ping parse(int ac, char **av) {
    t_ping res = {0};

    for (int i = 1; i < ac; i++) {
        if (av[i][0] == '-') {
            if (!av[i][1]) {
                fprintf(stderr, "Error: Invalid flag\n");
                exit(EXIT_FAILURE);
            }
            res.flags |= get_options(av[i] + 1);
        } else {
            if (res.host) {
                fprintf(stderr, "Error: Invalid flag\n");
                exit(EXIT_FAILURE);
            }
            res.host = av[i];
        }
    }
    res.addr = get_addr_info(res.host);
    // res->dest = get_dest(res->addr);
    return (res);
}