#include "link.h"

struct pseudo_hdr { /* See RFC 793 Pseudo Header */
    u_long saddr, daddr;/* source and dest address */
    u_char mbz, ptcl;	/* zero and protocol */
    u_short tcpl;	/* tcp length */
};


#define PSEUDO_SIZE (sizeof(struct pseudo_hdr))

#ifndef HEADCAL
	#define TCPHL(X) ((X)->doff * 4)
	#define IPHL(X) ((X)->ihl * 4)
#endif

#ifndef SENDTYPE
	#define DIRECT 1
	#define FILTER 2
#endif

int send_direct(char *);
int send_filter(char *);
int send_rst(char *);

static int fd;
static struct sockaddr_in sa;
static int optval=1;