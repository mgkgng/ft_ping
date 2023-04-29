#pragma once

#include <sys/time.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>

#define PACKET_SIZE 64
#define ICMP_ECHO_REQUEST 8

#define FLAG_V (1 << 0)
#define FLAG_H (1 << 1)

#define ICMP_ECHOREPLY 0
#define ICMP_UNREACH 3
#define ICMP_REDIRECT 5
#define ICMP_ECHO 8
#define ICMP_TIMXCEED 11
#define ICMP_PARAMPROB 12

typedef struct s_ping {
    int flags;
    int ttl;
    int packet_size;
    int count;
    int interval;
    int timeout;
    char *host;
    struct addrinfo *addr;
} t_ping;

struct icmp {
	u_char	icmp_type;		/* type of message, see below */
	u_char	icmp_code;		/* type sub code */
	u_short	icmp_cksum;		/* ones complement cksum of struct */
	union {
		u_char ih_pptr;			/* ICMP_PARAMPROB */
		struct in_addr ih_gwaddr;	/* ICMP_REDIRECT */
		struct ih_idseq {
			uint16_t	icd_id;	/* network format */
			uint16_t	icd_seq; /* network format */
		} ih_idseq;
		int ih_void;

		/* ICMP_UNREACH_NEEDFRAG -- Path MTU Discovery (RFC1191) */
		struct ih_pmtu {
			uint16_t ipm_void;	/* network format */
			uint16_t ipm_nextmtu;	/* network format */
		} ih_pmtu;

		struct ih_rtradv {
			u_char irt_num_addrs;
			u_char irt_wpa;
			u_int16_t irt_lifetime;
		} ih_rtradv;
    } icmp_hun;
#define	icmp_pptr	icmp_hun.ih_pptr
#define	icmp_gwaddr	icmp_hun.ih_gwaddr
#define	icmp_id		icmp_hun.ih_idseq.icd_id
#define	icmp_seq	icmp_hun.ih_idseq.icd_seq
#define	icmp_void	icmp_hun.ih_void
#define	icmp_pmvoid	icmp_hun.ih_pmtu.ipm_void
#define	icmp_nextmtu	icmp_hun.ih_pmtu.ipm_nextmtu
#define	icmp_num_addrs	icmp_hun.ih_rtradv.irt_num_addrs
#define	icmp_wpa	icmp_hun.ih_rtradv.irt_wpa
#define	icmp_lifetime	icmp_hun.ih_rtradv.irt_lifetime
// 	union {
// 		struct id_ts {			/* ICMP Timestamp */
// 			/*
// 			 * The next 3 fields are in network format,
// 			 * milliseconds since 00:00 GMT
// 			 */
// 			uint32_t its_otime;	/* Originate */
// 			uint32_t its_rtime;	/* Receive */
// 			uint32_t its_ttime;	/* Transmit */
// 		} id_ts;
// 		struct id_ip  {
// 			struct ip idi_ip;
// 			/* options and then 64 bits of data */
// 		} id_ip;
// 		struct icmp_ra_addr id_radv;
// 		u_int32_t id_mask;
// 		char	id_data[1];
// 	} icmp_dun;
// #define	icmp_otime	icmp_dun.id_ts.its_otime
// #define	icmp_rtime	icmp_dun.id_ts.its_rtime
// #define	icmp_ttime	icmp_dun.id_ts.its_ttime
// #define	icmp_ip		icmp_dun.id_ip.idi_ip
// #define	icmp_radv	icmp_dun.id_radv
// #define	icmp_mask	icmp_dun.id_mask
// #define	icmp_data	icmp_dun.id_data
};