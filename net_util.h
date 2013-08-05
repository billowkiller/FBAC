#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

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
//extern int iskeyword(char *, sqlite3 *);
extern int kw_match(char *str);

static struct HTTP post_H;
static long seq = 0L;
static long dseq = 0L;
