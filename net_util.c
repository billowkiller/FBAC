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

int tcp_type(struct iphdr *iph)
{
	struct tcphdr *tcph = (struct tcphdr *)((char *)iph + iph->ihl * 4);
	if(tcph->syn & ~(tcph->ack) & ~(tcph->rst))
		return FIRSTSHARK;
	
	if(tcph->syn & tcph->ack)
		return SECONDSHARK;
		
	if(~(tcph->syn) & tcph->ack & ~(tcph->psh))
		return THIRDSHARK;
	
	if((~(tcph->syn) & tcph->ack)
		&& 40 == ntohs(iph->tot_len))
		return ACK;

	if(ntohs(iph->tot_len) - IPHL(iph) - TCPHL(tcph))
	{
		char *payload = (char *)tcph + TCPHL(tcph);
		if(!strncmp(payload, "GET", 3))
			return GET;
		printf("%s\n", payload);
		if(!strncmp(payload, "POST", 4))
			return POST;
	}
	return 0;
}

int content_filter(struct iphdr *iph)
{
	struct tcphdr *tcph = (struct tcphdr *)((char *)iph + iph->ihl * 4);
	char *payload = (char *)tcph + TCPHL(tcph);
	char *start = strchr(payload, ' ');
	int i=1;
	while(*(start+i)!=' ')
	{
		if(*(start+i) == '.')
			return TRUE;
		i++;
	}
	
	return FALSE;
}

int ishost(struct iphdr *iph, char *hostname)
{
	struct tcphdr *tcph = (struct tcphdr *)((char *)iph + iph->ihl * 4);
	char * payload = (char *)tcph + TCPHL(tcph);
	char *result = strstr(payload, "Host");
	if(result && !strncmp(result+6, hostname, strlen(hostname)))
		return TRUE;
	return FALSE;
}

int _is_forbiden_url(char *data)
{
	int url_len = 0;
	struct iphdr *iph = (struct iphdr *)data;
	struct tcphdr *tcph = (struct tcphdr *)(data + iph->ihl * 4);
	char * payload = strchr((char *)tcph + tcph->doff*4, '/');
	while( *(payload + ++url_len) != ' ');
	char * url = (char *)malloc(url_len);
	strncpy(url, payload, url_len);
	printf("%s\n", url);
	if(!strcmp(url, "/search?q=nihao"))
		return TRUE;
	return FALSE;
}

int _replace(char *data)
{
	struct iphdr *iph = (struct iphdr *)data;
	struct tcphdr *tcph = (struct tcphdr *)(data + iph->ihl * 4);
	char * payload = data + iph->ihl*4 + tcph->doff*4;
	char *pattern = "billowkiller";
	char *result = strstr(payload, pattern);
	if(NULL != result)
		memset(result, '*', strlen(pattern));
	result = strstr(payload, "Accept-Encoding");
	if(NULL != result) //plain txt
	{	
		result += 17;
		while(*result != '\r')
			*result++ = ' ';
	}
	printf("payload = %s\n", payload);
	return 0;
}

int send_data(char *data, int flag)
{
	switch(flag)
	{
		case SEND_DIRECT: 
			send_direct(data);
			break;
		case SEND_GET:
			if(_is_forbiden_url(data))
			{
				printf("rst\n");
				send_rst(data);
			}
			else 
			{	
				printf("_replace\n");
				_replace(data);
				send_filter(data);
			}
			break;
	}
	return 0;
}