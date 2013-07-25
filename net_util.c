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

	return 0;
}

char* _url(char *payload)
{
	char * first = strchr(payload, '/');
	int len = strchr(first, ' ') - first;
	char *url = (char *)malloc(len);
	memcpy(url, first, len);
	return url;
}

//according to host, not url
int _page_type(char *host)
{
	if(!strncmp(host, "shell", 5))
		return STATUS;
	else if(!strncmp(host, "friend", 6))
		return FRIEND;
	else if(!strncmp(host, "blog", 4))
		return NOTE;
	else if(!strncmp(host, "comment", 7))
		return COMMENT;
	else if(!strncmp(host, "browse", 6))
		return BROWSE;
	else
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
	char *host = (char *)malloc(20);
	if(result)
		memcpy(host, result+6, 20);
	if(result && strstr(host, hostname))
	{
		free(host);
		return TRUE;
	}
	return FALSE;
//	if(result && !strncmp(result+6, hostname, strlen(hostname)))
//		return TRUE;
//	return FALSE;
}

int _check_blocklist(char *str)
{
	GSList *list = g_hash_table_lookup(hash_config,"block_list");
	GSList *iterator = NULL;
	for (iterator = list; iterator; iterator = iterator->next)
	{
		if(strstr(str, (char*)iterator->data))
			return TRUE;
	}
	return FALSE;
}

char *_check_sslist(char *str)
{
	GSList *list = g_hash_table_lookup(hash_config,"sensetive_list");
	GSList *iterator = NULL;
	char *position = NULL;
	for (iterator = list; iterator; iterator = iterator->next)
	{
		position = strstr(str, (char*)iterator->data);
		break;
	}
	return position;
}

char *_check_strlist(char *str)
{
	GSList *list = g_hash_table_lookup(hash_config,"sensetive_string");
	GSList *iterator = NULL;
	char *position = NULL;
	int len;
	for (iterator = list; iterator; iterator = iterator->next)
	{
		printf("..........match:%s......\n", (char*)iterator->data);
//		_match((char*)iterator->data, str, &position, &len);
		printf("..........match finish, len=%d.........\n", len);
		//position = strstr(str, (char*)iterator->data);
		if(position)
		{
			memset(position, '*', len);
			break;
		}
	}
	return position;
}

int send_data(char *data, int flag)
{
	struct iphdr *iph = (struct iphdr *)data;
	struct tcphdr *tcph = (struct tcphdr *)(data + IPHL(iph));
	int http_len = IPL(iph) - IPHL(iph) - TCPHL(tcph);
	char * payload = data + TCPHL(tcph) + IPHL(iph);
	
	int n;
	switch(flag)
	{
		case SEND_DIRECT: 
			send_direct(data);
			break;
		case SEND_UP:
			if(payload)
			{
				if(!processhttp(payload, http_len))
					send_rst(data);
				n = find_db(c_info.s_id, c_info.user_id, c_info.p_type, c_info.r_id, db);
				if(n)
				{
					printf("find num = %d\n", n);
					send_rst(data);
				}
			}
			
		//	if(_check_blocklist(http.url))
		//	{
		//		send_rst(data);
		//		printf("rst\n");
		//	}
		//	else if(_check_strlist(http.url))
		//		send_filter(data);
		//	else 
		//		send_filter(data);
			
			//handle url and free http, not cookie this time
			break;
	}
	return 0;
}
