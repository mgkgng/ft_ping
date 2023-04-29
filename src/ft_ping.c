#include "ft_ping.h"
#include "libft.h"

t_ping *g_ping_info;
int g_sockfd;
struct icmp g_packet;

void exit_error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int get_options(char *flag_str) {
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

void parse(int ac, char **av) {
    g_ping_info = ft_calloc(1, sizeof(t_ping));

    for (int i = 1; i < ac; i++) {
        if (av[i][0] == '-') {
            if (!av[i][1]) {
                fprintf(stderr, "Error: Invalid flag\n");
                exit(EXIT_FAILURE);
            }
            g_ping_info->flags |= get_options(av[i] + 1);
        } else {
            if (g_ping_info->host) {
                fprintf(stderr, "Error: Invalid flag\n");
                exit(EXIT_FAILURE);
            }
            g_ping_info->host = av[i];
        }
    }
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
    } else if (g_ping_info->addr->ai_family != AF_INET) {
        fprintf(stderr, "Not a valid IPv4 address\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in *ipv4 = (struct sockaddr_in *) g_ping_info->addr->ai_addr;

    char ipstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipv4->sin_addr, ipstr, INET_ADDRSTRLEN);
    printf("IP address: %s\n", ipstr);
    inet_pton(AF_INET, ipstr, &ipv4->sin_addr);
    printf("IP address: %s\n", ipstr);
}

void terminate() {
    freeaddrinfo(g_ping_info->addr);
    free(g_ping_info);
}

void init_socket() {
    // Ref: https://courses.cs.vt.edu/cs4254/fall04/slides/raw_1.pdf
    // Ref2: https://www.techtarget.com/searchnetworking/definition/time-to-live#:~:text=Time%2Dto%2Dlive%20(TTL)%20is%20a%20value%20for,lifetime%2C%20depends%20on%20the%20context.
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        fprintf(stderr, "ft_ping: socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("Socket created.\n");

    // Set socket options to enable ICMP protocol
    const int ttl = 255;
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        fprintf(stderr, "ft_ping: setsockopt: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("Socket options set.\n");
}

void create_packet() {
    // Ref: https://courses.cs.vt.edu/cs4254/fall04/slides/raw_1.pdf
    g_packet.icmp_type = ICMP_ECHO;
    g_packet.icmp_code = 0;
    g_packet.icmp_id = getpid();
    g_packet.icmp_seq = 0;
    g_packet.icmp_cksum = 0;
}

int main(int ac, char **av) {
    if (ac < 2)
        exit(0);
    parse(ac, av);
    get_address();
    init_socket();
    create_packet();    

    

    while (1) {
        // send ping to the address
        int ret = sendto(g_sockfd, &packet, sizeof(struct icmp), 0, (struct sockaddr *)ipv4, sizeof(struct sockaddr_in)); 
        if (ret < 0)
            exit_error("ft_ping: sendto");
        usleep(1000000);
    }

    terminate();
}