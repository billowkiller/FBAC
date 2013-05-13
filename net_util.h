#include <stdio.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <string.h>
#include "data_send.h"
#include "stringProcess.h"

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef TCPTYPE
	#define FIRSTSHARK -1
	#define SECONDSHARK -2
	#define THIRDSHARK -3
	#define ACK 1
	#define GET 2
	#define POST 3
#endif

#ifndef HEADCAL
	#define TCPHL(X) ((X)->doff * 4)
	#define IPHL(X) ((X)->ihl * 4)
#endif

#define SEND_DIRECT 0
#define SEND_GET 1

static long seq = 0;

int isFromDest(uint32_t);
int isFromSrc(struct iphdr *, char *);

int tcp_type(struct iphdr *);

int send_data(char *, int);