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
		
// 	if(~(tcph->syn) & tcph->ack & ~(tcph->psh))
// 		return THIRDSHARK;
// 	
// 	if((~(tcph->syn) & tcph->ack)
// 		&& 40 == ntohs(iph->tot_len))
// 		return ACK;

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

int send_data(char *data)
{
	struct iphdr *iph = (struct iphdr *)data;
	struct tcphdr *tcph = (struct tcphdr *)(data + IPHL(iph));
	int http_len = IPL(iph) - IPHL(iph) - TCPHL(tcph);
	
	if(http_len == 0)
		return 1;
	if(dseq == ntohl(tcph->seq))
		return 0;
	char * payload = data + TCPHL(tcph) + IPHL(iph);
	
	int n = 0;
	//tcp fragment coming, more than one 
	if(seq+post_H.head_length == ntohl(tcph->seq))
	{
		char *content = malloc(post_H.head_length+http_len);
		memcpy(content, post_H.content, post_H.head_length);
		memcpy(content+post_H.head_length, payload, http_len);
		#ifdef DEBUG
		printf("*****************\n%s\n", content);
		#endif
		if(!processhttp(content, post_H.head_length+http_len))
		{
			post_H.head_length += http_len;
			memcpy(post_H.content, content, post_H.head_length);  //not so efficent, but ok
		}else{
			if(kw_match(c_info.comment))
			{
		//		post_H.head_length -= http_len;
				free(content); //need delete
				dseq = ntohl(tcph->seq);
printf("keywords match\n");
				return 0;
			}
			n = find_db(c_info.user_id, c_info.s_id, c_info.p_type, c_info.r_id);
			if(n)
			{
				#ifdef DEBUG
				printf("find num = %d\n", n);
				#endif
				free(content); //need delete
				dseq = ntohl(tcph->seq);
				return 0;
			}
		}
		free(content);
	}
	else if(http_len)
	{
		if(!processhttp(payload, http_len))
		{
			seq = ntohl(tcph->seq);
			http.head_length = http_len;
			memcpy(&post_H, &http, sizeof(struct HTTP));
		}
		if(kw_match(c_info.comment)){
printf("keywords match\n");
			return 0;
}
		if(c_info.user_id[0]!='\0')
			n = find_db(c_info.user_id, c_info.s_id, c_info.p_type, c_info.r_id);
		if(n)
		{
			#ifdef DEBUG
			printf("find num = %d\n", n);
			#endif
			return 0;
		}
	}
			
	return 1;
}
