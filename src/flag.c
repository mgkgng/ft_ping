#include "ft_ping.h"

// Flag C
int get_count(char *s) {
    int count = ft_atoi(s, 1);
    if (count <= 0) {
        fprintf(stderr, "ping: invalid count of packets to transmit: '%s'\n", s);
        exit(EXIT_FAILURE);
    }
    return (count);
}

// Flag I
float get_interval(char *s) {
    int is_float = ft_isfloat(s);
    if (!is_float) {
        fprintf(stderr, "ping: invalid timing interval: '%s'\n", s);
        exit(EXIT_FAILURE);
    }
    float interval = atof(s);
    if (interval <= 0) {
        fprintf(stderr, "ping: invalid timing interval: '%s'\n", s);
        exit(EXIT_FAILURE);
    }
    return (interval);
}