#include "ft_ping.h"
#include "libft.h"

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

void check_address(char *hostname) {
    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo *res;
    int ret = getaddrinfo(hostname, NULL, &hints, &res);
    if (ret != 0) {
        printf("ft_ping: cannot resolve %s: Unknown host\n", hostname);
        exit(EXIT_FAILURE);
    }

    // Iterate over the list of addresses and print them
    for (struct addrinfo *rp = res; rp != NULL; rp = rp->ai_next) {
        char addrstr[INET6_ADDRSTRLEN];
        void *addrptr = NULL;

        if (rp->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)rp->ai_addr;
            addrptr = &ipv4->sin_addr;
        } else {
            fprintf(stderr, "Unknown address family: %d\n", rp->ai_family);
            continue;
        }

        inet_ntop(rp->ai_family, addrptr, addrstr, sizeof(addrstr));
        printf("%s\n", addrstr);
    }
    freeaddrinfo(res);
}

int main(int ac, char **av) {
    if (ac < 2)
        exit(0);
    t_ping *ping_info = parse(ac, av);    
    check_address(ping_info->host);
}