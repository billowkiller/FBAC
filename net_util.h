#include <stdio.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <string.h>
#include <glib.h>
#include <sqlite3.h>
#include "data_send.h"
#include "include.h"

extern GHashTable* hash_config;
extern struct HTTP http;
extern sqlite3* db;
extern struct connection_info c_info;
extern int processhttp(char *, int);
extern int find_db(char *, char *, int, char *, sqlite3 *);

static struct HTTP post_H;
static long seq = 0;

int isFromDest(uint32_t);
int isFromSrc(struct iphdr *, char *);

int tcp_type(struct iphdr *);
int content_filter(struct iphdr *);
int ishost(struct iphdr *, char *);
int send_data(char *, int);
