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

unsigned short get_checksum(unsigned short *buffer, int length) {
    unsigned long sum = 0;
    while (length > 1) {
        sum += *buffer++;
        length -= 2;
    }
    if (length == 1)
        sum += *(unsigned char *)buffer;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

void get_local_ip() {
    struct addrinfo hints, *res, *p;
    char host[NI_MAXHOST];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;

    // Get local address info
    if (getaddrinfo("localhost", NULL, &hints, &res) != 0)
        exit_error("getaddrinfo: Failed to get local address info\n");

    for (p = res; p != NULL; p = p->ai_next) {
        void* addr;
        if (p->ai_family == AF_INET) {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } else
            continue;
        if (inet_ntop(p->ai_family, addr, host, sizeof host) != NULL) {
            freeaddrinfo(res);
            strcpy(g_local_ip, host);
            return ;
        }
    }
    freeaddrinfo(res);
    fprintf(stderr, "Failed to retrieve local IP address\n");
    exit(EXIT_FAILURE);
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
    struct addrinfo hints, *res, *p;
    char ipstr[INET_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int ret = getaddrinfo(g_ping_info->host, NULL, &hints, &res);
    if (ret != 0) {
        fprintf(stderr, "ft_ping: cannot resolve %s: Unknown host\n", g_ping_info->host);
        exit(EXIT_FAILURE);
    } else if (res->ai_family != AF_INET) {
        fprintf(stderr, "Not a valid IPv4 address\n");
        exit(EXIT_FAILURE);
    }

    for (p = res; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) {  // IPv4
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
            if (inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, sizeof ipstr) != NULL) {
                freeaddrinfo(res);
                strcpy(g_dest_ip, ipstr);
            }
        }
    }

    freeaddrinfo(res);
    fprintf(stderr, "Failed to retrieve IPv4 address for %s\n", g_ping_info->host);
    exit(EXIT_FAILURE);


    // struct sockaddr_in *ipv4 = (struct sockaddr_in *) g_ping_info->addr->ai_addr;

    // char ipstr[INET_ADDRSTRLEN];
    // inet_ntop(AF_INET, &ipv4->sin_addr, ipstr, INET_ADDRSTRLEN);
    // printf("IP address: %s\n", ipstr);
    // inet_pton(AF_INET, ipstr, &ipv4->sin_addr);
    // printf("IP address: %s\n", ipstr);
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

void create_packet(char **packet) {
    // Ref: https://courses.cs.vt.edu/cs4254/fall04/slides/raw_1.pdf

    memset(packet, 0, PACKET_SIZE);

    // Linux version
    // struct iphdr *ip_header = (struct iphdr *) packet;
    // ip_header->ihl = 5;
    // ip_header->version = 4;
    // ip_header->tos = 0;
    // ip_header->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr);
    // ip_header->id = htons(g_id++);
    // ip_header->frag_off = 0;
    // ip_header->ttl = 255;
    // ip_header->protocol = IPPROTO_ICMP;
    // ip_header->check = 0;
    // ip_header->saddr = 0;
    // ip_header->daddr = ((struct sockaddr_in *)g_ping_info->addr->ai_addr)->sin_addr.s_addr;
    // ip_header->check = get_checksum((unsigned short *)packet, ip_header->tot_len);

    // MacOS version (after Catalina)
    struct ip *ip_header = (struct ip *) packet;
    ip_header->ip_hl = 5;
    ip_header->ip_v = 4;
    ip_header->ip_tos = 0;
    ip_header->ip_len = sizeof(struct ip) + sizeof(struct icmphdr);
    ip_header->ip_id = htons(getpid());
    ip_header->ip_off = 0;
    ip_header->ip_ttl = 64;
    ip_header->ip_p = IPPROTO_ICMP;
    ip_header->ip_src.s_addr = inet_addr(g_local_ip);
    ip_header->ip_dst.s_addr = inet_addr(g_dest_ip);
    ip_header->ip_sum = 0;
    ip_header->ip_sum = get_checksum((unsigned short *)ip_header, sizeof(struct ip));

    struct icmphdr *icmp_header = (struct icmphdr *) (packet + sizeof(struct ip));
    icmp_header->type = ICMP_ECHO;
    icmp_header->code = 0;
    icmp_header->checksum = 0;
    icmp_header->un.echo.id = htons(getpid());
    icmp_header->un.echo.sequence = htons(1);
    icmp_header->checksum = get_checksum((unsigned short *)icmp_header, ICMP_HEADER_SIZE);

}

int main(int ac, char **av) {
    if (ac < 2)
        exit(0);
    parse(ac, av);
    get_address();
    get_local_ip();
    init_socket();

    char packet[PACKET_SIZE];
    while (1) {
        create_packet((char **) &packet);
        // send ping to the address
        // int ret = sendto(g_sockfd, &packet, sizeof(struct icmp), 0, (struct sockaddr *)ipv4, sizeof(struct sockaddr_in)); 
        // if (ret < 0)
        //     exit_error("ft_ping: sendto");
        printf("ding dong\n");
        usleep(1000000);
    }

    terminate();
}