/*
 * =====================================================================================
 *
 *       Filename:  data_send.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/25/2013 04:19:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Billowkiller (bk), billowkiller@gmail.com
 *   Organization:
 *
 * =====================================================================================
 */
#include "data_send.h"

#define PSEUDO_SIZE (sizeof(struct pseudo_hdr))

//checksum algo.
inline u_short in_cksum(u_short *addr, int len)
{
    register int nleft = len;
    register u_short *w = addr;
    register int sum = 0;
    u_short answer = 0;

     /* Our algorithm is simple, using a 32 bit accumulator (sum), we add
      * sequential 16 bit words to it, and at the end, fold back all the
      * carry bits from the top 16 bits into the lower 16 bits. */

     while (nleft > 1) {
         sum += *w++;
         nleft -= 2;
     }

     /* mop up an odd byte, if necessary */
     if (nleft == 1) {
         *(u_char *)(&answer) = *(u_char *) w;
         sum += answer;
     }

     /* add back carry outs from top 16 bits to low 16 bits */
     sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
     sum += (sum >> 16); /* add carry */
     answer = ~sum; /* truncate to 16 bits */
     return(answer);
}

//recaculate ip and tcp checksum
int _recal_cksum(char **d)
{
	char *data = *d;
	struct iphdr *iph = (struct iphdr *)data;
	struct tcphdr *tcph = (struct tcphdr *)(data + IPHL(iph));
	int datalen = ntohs(iph->tot_len) - IPHL(iph) - TCPHL(tcph);

	char * pseudo = (char *)malloc(PSEUDO_SIZE + TCPHL(tcph) + datalen);

	//ip checksum
    iph->check = 0;
    iph->check = in_cksum((u_short *)iph, IPHL(iph));
	//tcp checksum
	struct pseudo_hdr * pseudo_h = (struct pseudo_hdr *)pseudo;
	tcph->check = 0;
	pseudo_h->saddr = iph->saddr;
	pseudo_h->daddr = iph->daddr;
	pseudo_h->mbz = 0;
    pseudo_h->ptcl = IPPROTO_TCP;
    pseudo_h->tcpl = htons(ntohs(iph->tot_len) - IPHL(iph));
    memcpy(pseudo + PSEUDO_SIZE, tcph, TCPHL(tcph));
   	memcpy(pseudo + PSEUDO_SIZE + TCPHL(tcph), data+IPHL(iph)+TCPHL(tcph), datalen);

   	tcph->check = in_cksum(pseudo, PSEUDO_SIZE + TCPHL(tcph) + datalen);
   	free(pseudo);

   	//end
   	return ntohs(iph->tot_len);
}
