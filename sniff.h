/*
	Packet sniffer using libpcap library
*/
#include <stdio.h>
#include <stdlib.h> // for exit()
#include <string.h> //for memset
#include <sys/socket.h>
#include <arpa/inet.h> // for inet_ntoa()
#include <net/ethernet.h>
#include <netinet/ip_icmp.h>	//Provides declarations for icmp header
#include <netinet/udp.h>	//Provides declarations for udp header
#include <netinet/tcp.h>	//Provides declarations for tcp header
#include <netinet/ip.h>	//Provides declarations for ip header
#include <glib.h>
#include <sqlite3.h>
#include <linux/netfilter.h>		/* for NF_ACCEPT */
#include <errno.h>

#include <libnetfilter_queue/libnetfilter_queue.h>

#include "include.h"

sqlite3 *db = NULL;

extern GHashTable* hash_config;

extern int isFromDest(uint32_t);
extern int isFromSrc(struct iphdr *, char *);
extern int tcp_type(struct iphdr *);
extern int content_filter(struct iphdr *);
extern int ishost(struct iphdr *, char *);
extern int send_data(char *);
extern int pipe_config();
extern void read_kw_file();
