#include <stdio.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <string.h>
#include <regex.h>
#include <glib.h>
#include "data_send.h"
#include "include.h"

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
