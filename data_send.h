#include "link.h"

struct pseudo_hdr { /* See RFC 793 Pseudo Header */
    u_long saddr, daddr;/* source and dest address */
    u_char mbz, ptcl;	/* zero and protocol */
    u_short tcpl;	/* tcp length */
	struct tcphdr tcp;
};


#define PSEUDO_SIZE (sizeof(struct pseudo_hdr))
#ifndef TCPHL
	#define TCPHL 20
#endif

#ifndef IPHL
	#define IPHL 20
#endif


int send_alldata(Node *);
int send_direct(struct iphdr *, struct tcphdr *);

static int fd;
static struct sockaddr_in sa;
static int optval=1;