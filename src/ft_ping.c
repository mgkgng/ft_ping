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
            res->flags |= get_option(av[i] + 1);
        } else {
            if (res->host)
                exit(0); //TODO error msg
            res->host = av[i];
        }
    }
}

int main(int ac, char **av) {
    if (ac < 2)
        exit(0);
    t_ping *ping_info = parse(ac, av);
}
