#include "ft_ping.h"

static int get_options(char *flag_str, t_ping *ping) {
    int res = 0;

    for (int i = 0; flag_str[i]; i++) {
        if (flag_str[i] == 'v')
            res |= FLAG_V;
        else if (flag_str[i] == 'h')
            res |= FLAG_H;
        else if (flag_str[i] == 'q')
            res |= FLAG_Q;
        else if (flag_str[i] == 'c') {
            res |= FLAG_C;
            if (flag_str[i + 1]) {
                ping->count = get_count(flag_str + i + 1);
                return (res);
            } else
                ping->check_param |= FLAG_C;
        } else if (flag_str[i] == 'i') {
            res |= FLAG_I;
            if (flag_str[i + 1]) {
                ping->interval = get_interval(flag_str + i + 1);
                return (res);
            } else
                ping->check_param |= FLAG_I;
        }
        else {
            fprintf(stderr, "Error: Invalid flag\n");
            exit(EXIT_FAILURE);
        }
    }
    return (res);
}

static int is_valid_ipv4(char *ip_str) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, ip_str, &(sa.sin_addr)) != 0;
}

static struct addrinfo *get_addr_info(char *host) {
    struct addrinfo hints, *res;
    
    if (is_valid_ipv4(host)) {
        res = (struct addrinfo*) malloc(sizeof(struct addrinfo));
        res->ai_family = AF_INET;
        res->ai_socktype = SOCK_RAW;
        res->ai_protocol = IPPROTO_ICMP;
        res->ai_addr = (struct sockaddr*) malloc(sizeof(struct sockaddr_in));
        res->ai_addr->sa_family = AF_INET;
        ((struct sockaddr_in*) res->ai_addr)->sin_addr.s_addr = inet_addr(host);
        res->ai_addrlen = sizeof(struct sockaddr_in);
        return (res);
    }

    ft_bzero(&hints, sizeof(hints));
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

static void get_dest(t_ping *ping) {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *) ping->addr->ai_addr;
    char ipstr[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &ipv4->sin_addr, ipstr, INET_ADDRSTRLEN);
    ft_strcpy(ping->dest, ipstr);
}

t_ping parse(int ac, char **av) {
    t_ping res = {0};

    for (int i = 1; i < ac; i++) {
        if (res.check_param) {
            if (res.check_param & FLAG_C) {
                res.count = get_count(av[i]);
                res.check_param ^= FLAG_C;
            } else if (res.check_param & FLAG_I) {
                res.interval = get_interval(av[i]);
                res.check_param ^= FLAG_I;
            }
        } else if (av[i][0] == '-') {
            if (!av[i][1]) {
                fprintf(stderr, "Error: Invalid flag\n");
                exit(EXIT_FAILURE);
            }
            res.flags |= get_options(av[i] + 1, &res);
            if (res.flags & FLAG_H) {
                print_help();
                exit(EXIT_SUCCESS);
            }
        } else {
            if (res.host) {
                fprintf(stderr, "Error: Invalid flag\n");
                exit(EXIT_FAILURE);
            }
            res.host = av[i];
        }
    }
    if (res.check_param) {
        fprintf(stderr, "ping: option requires an argument -- '%c'\n", (res.check_param == FLAG_C) ? 'c' : 'i');
        exit(EXIT_FAILURE);
    }
    res.addr = get_addr_info(res.host);
    get_dest(&res);
    return (res);
}