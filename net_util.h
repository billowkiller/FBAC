#include <stdio.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <string.h>
#include "data_send.h"
#include <regex.h>
#include <glib.h>

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
	#define IPL(X) (ntohs((X)->tot_len))
#endif

#define SEND_DIRECT 0
#define SEND_GET 1
#define SEND_POST 2

#define FRIEND 1
#define STATUS 2
#define BLOG 3
#define COMMENT 4
#define BROWSE 5

struct HTTP{
	char url[100];
	char host[20];
	char cookie[300];
	char content[200];
};

extern GHashTable* hash_config;
extern struct HTTP http;
extern int processhttp(char *, int);

static long seq = 0;

int isFromDest(uint32_t);
int isFromSrc(struct iphdr *, char *);

int tcp_type(struct iphdr *);
int content_filter(struct iphdr *);
int ishost(struct iphdr *, char *);
int send_data(char *, int);
