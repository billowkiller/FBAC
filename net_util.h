#include <stdio.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <string.h>
#include "data_send.h"
#include "stringProcess.h"

#ifndef DESTHOST
	#define DESTHOST "www.douban.com"
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FIRSTSHARK
	#define FIRSTSHARK 1
#endif

#ifndef SECONDSHARK
	#define SECONDSHARK 2
#endif

#ifndef THIRDSHARK
	#define THIRDSHARK 3
#endif

#define SEND_DIRECT 0
#define SEND_FILTER 1

static long seq = 0;

int isFromDest(uint32_t);
int isFromSrc(struct iphdr *, char *);

int handshark(struct iphdr *, struct tcphdr *);

int is_response_ack(struct iphdr *, struct tcphdr *);

int send_data(char *, int);