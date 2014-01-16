//#define DEBUG   //define debug print
#define NDEBUG   //shutdown assert

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/socket.h>
#include <arpa/inet.h> // for inet_ntoa()
#include <net/ethernet.h>
#include <netinet/ip_icmp.h>	//Provides declarations for icmp header
#include <netinet/udp.h>	//Provides declarations for udp header
#include <netinet/tcp.h>	//Provides declarations for tcp header
#include <netinet/ip.h>	//Provides declarations for ip header
#include <linux/netfilter.h>		/* for NF_ACCEPT */
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define TCPH(X) ((char *)(X)+IPHL(X)) //ip jump to tcp
#define TCPHL(X) (((struct tcphdr *)(X))->doff * 4) //tcp header length
#define IPHL(X) (((struct iphdr *)(X))->ihl * 4)  //ip header length
#define IPL(X) (ntohs(((struct iphdr *)(X))->tot_len)) //ip length
#define PAYLOAD(X) ((char *)(X)+IPHL(X)+TCPHL(TCPH(X))) //payload length
#define PAYLOADL(X) (IPL(X)-IPHL(X)-TCPHL(TCPH(X))) //payload length
#define SEQ(X) (ntohl(((struct tcphdr *)(X))->seq)) //tcp seq number

struct HTTP{
	unsigned char method;
	int head_length;
	char url[300];
	char cookie[300];
	char content[4096];
};

struct connection_info{
	char user_id[20];
	char s_id[20]; //visitor view subject's page
	int p_type; /* page type */
	char r_id[52]; /* resource id */
	char comment[100]; /* post content */
};
