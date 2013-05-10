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

int isFromSrc(struct iphdr *iph, char *ip)
{
	struct sockaddr_in source;

	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iph->saddr;

	if(strcmp(ip, inet_ntoa(source.sin_addr)))
		return 0;
	else
		return 1;
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
		
	if((~(tcph->syn) & tcph->ack & ~(tcph->psh))
		&& seq == ntohl(tcph->seq) - 1)
		return THIRDSHARK;
		
	return 0;
}

int is_response_ack(struct iphdr *iph, struct tcphdr *tcph)
{
	if((~(tcph->syn) & tcph->ack)
		&& 40 == ntohs(iph->tot_len))
		return TRUE;
	return FALSE;
}

int _replace(char *data)
{
	struct iphdr *iph = (struct iphdr *)data;
	struct tcphdr *tcph = (struct tcphdr *)(data + iph->ihl * 4);
	char * payload = data + iph->ihl*4 + tcph->doff*4;
	char *pattern = "billowkiller";
	char *result = kmp_search(payload, pattern);
	if(NULL != result)
		memset(result, '*', strlen(pattern));
	return 0;
}

int send_data(char *data, int flag)
{
	switch(flag)
	{
		case SEND_DIRECT: 
			send_direct(data);
			break;
		case SEND_FILTER:
			_replace(data);
			send_filter(data);
			break;
	}
	return 0;
}