/*
 * =====================================================================================
 *
 *       Filename:  net_util.c
 *
 *    Description:  network utility
 *					-	check ip addr is from destination
 *					-	assert tcp hand shark
 *
 *        Version:  1.0
 *        Created:  04/30/2013 03:38:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Billowkiller (bk), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "net_util.h"

int isFromDest(uint32_t net)
{
	//printf("%s\n", libnet_addr2name4(net, LIBNET_RESOLVE));
	return 0;
}

int handshark(struct iphdr *iph, struct tcphdr *tcph)
{
	if(tcph->syn & ~(tcph->ack) & ~(tcph->rst))
	{
		seq = ntohl(tcph->seq);
		return FIRSTSHARK;
	}
	if(tcph->syn & tcph->ack)
		return SECONDSHARK;
		
	if((~(tcph->syn) & tcph->ack) 
		&& seq == ntohl(tcph->seq) - 1 
		&& 40 == ntohs(iph->tot_len))
		return THIRDSHARK;
		
	return 0;
}

int response_ack(struct iphdr *iph, struct tcphdr *tcph)
{
	if((~(tcph->syn) & tcph->ack)
		&& 40 == ntohs(iph->tot_len))
		return TRUE;
	return FALSE;
}