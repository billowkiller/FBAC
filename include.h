//#define DEBUG   //define debug print
//#define NDEBUG   //shutdown assert

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

#define PR(X, Y) printf(#X " = " Y "\n", X ) //i=1; PR(i, "%d")
#define PS(X) printf(#X " = %s\n", X ) //PR(str)
#define PD(X) printf(#X " = %d\n", X ) //PD(int)

#define TCPH(X) ((char *)(X)+IPHL(X)) //ip jump to tcp
#define TCPHL(X) (((struct tcphdr *)(X))->doff * 4) //tcp header length
#define OPTIONL(X) (TCPHL(TCPH(X))-20)
#define IPHL(X) (((struct iphdr *)(X))->ihl * 4)  //ip header length
#define IPL(X) (ntohs(((struct iphdr *)(X))->tot_len)) //ip length
#define PAYLOAD(X) ((char *)(X)+IPHL(X)+TCPHL(TCPH(X))) //payload length
#define PAYLOADL(X) (IPL(X)-IPHL(X)-TCPHL(TCPH(X))) //payload length
#define SEQ(X) (ntohl(((struct tcphdr *)(X))->seq)) //uint32_t tcp seq number
#define ACK(X) (ntohl(((struct tcphdr *)(X))->ack_seq)) //tcp ack number

typedef struct{
    int http_len;  //origin http length
    int head_len;  //response header length
    int MSS;
    long last_seq; //last payload tcp seq
}SessionData;
