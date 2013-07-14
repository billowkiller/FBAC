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
//		printf("%s\n", payload);
		if(!strncmp(payload, "POST", 4))
			return POST;
	}
	return 0;
}

char* urldecode(char *cp)
{
    char *p=(char*)malloc((strlen(cp)+1));
    memcpy(p,cp,strlen(cp)+1);
    char *pr=p;
    register i=0;
    while(*(p+i))
    {
       if ((*p=*(p+i)) == '%')
       {
        *p=*(p+i+1) >= 'A' ? ((*(p+i+1) & 0XDF) - 'A') + 10 : (*(p+i+1) - '0');//0xDF按位与是将小写字母转成大写大写
        *p=(*p) * 16;
        *p+=*(p+i+2) >= 'A' ? ((*(p+i+2) & 0XDF) - 'A') + 10 : (*(p+i+2) - '0');
        i+=2;
       }
       else if (*(p+i)=='+')
       {
        *p=' ';
       }
       p++;
    }
    *p='\0';
    return pr;
}

char* _url(char *payload)
{
	char * first = strchr(payload, '/');
	int len = strchr(first, ' ') - first;
	char *url = (char *)malloc(len);
	memcpy(url, first, len);
	return url;
}

int _page_type(char *payload)
{
	char *host = strstr(payload, "Host");
	if(!host)
		return 0;
	host += 6; //Host: url
	if(!strncmp(host, "shell", 5))
		return STATUS;
	else if(!strncmp(host, "friend", 6))
		return FRIEND;
	else if(!strncmp(host, "blog", 4))
		return BLOG;
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
	if(result && strstr(result+6, hostname))
		return TRUE;
	return FALSE;
//	if(result && !strncmp(result+6, hostname, strlen(hostname)))
//		return TRUE;
//	return FALSE;
}

int _is_forbiden(char *data)
{
	struct iphdr *iph = (struct iphdr *)data;
	struct tcphdr *tcph = (struct tcphdr *)(data + iph->ihl * 4);
	char * payload = data + iph->ihl*4 + tcph->doff*4;
	//printf("%s", payload);
	int len = strchr(payload, '\r')-payload;
	char * firstline = (char *)malloc(len);
	memcpy(firstline, payload, len);
	if(strstr(firstline, "344324313"))
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
//	printf("payload = %s\n", payload);
	return 0;
}

int _status_filter(char *payload)
{
	char *result = strstr(payload, "\r\n\r\n")+4;
	if(strstr(result, "528498149"))
	{
		printf("%s\n", result);
		char *first = strchr(result, '=');
		char *last = strchr(result, '&');
		char *content = (char *)malloc(last - first-1);
		memcpy(content, first+1, last-first-1);
		printf("%s\n", content);
		free(content);
		memset(first+1, '*', last-first-1);
		return TRUE;
	}
	return FALSE;
}

int _browse_filter(char *payload, int get_or_post)
{
	if(get_or_post == SEND_GET)
	{
		char *url = _url(payload);              //need free
		char *query = strstr(url, "q=");
		if(query)
		{
			query += 2;
			char *d_query = urldecode(query);		//need free
			if(!strncmp(d_query, "吴涛", 4))
				return FALSE; 
		}
	}else
	{
		char *result = strstr(payload, "\r\n\r\n")+4;
	}
	return TRUE;
	
}

int send_data(char *data, int flag)
{
	struct iphdr *iph = (struct iphdr *)data;
	struct tcphdr *tcph = (struct tcphdr *)(data + IPHL(iph));
	char * payload = data + TCPHL(tcph) + IPHL(iph);
	switch(flag)
	{
		case SEND_DIRECT: 
			send_direct(data);
			break;
		case SEND_GET:
		printf("%s", _url(payload));              //need free
			if(_is_forbiden(data))
			{
				printf("rst\n");
				send_rst(data);
			}
			else if(!_browse_filter(payload, SEND_GET)) 
			{	
				printf("rst\n");
				send_rst(data);
			}else
			{
				_replace(data);
				printf("yes\n");
				send_filter(data);
			}
			break;
		case SEND_POST:
			switch(_page_type(payload))
			{
				case STATUS:
//					_status_filter(payload);
					break;
				case BROWSE:
					break;
				case BLOG:
					break;
				case COMMENT:
					break;
				case FRIEND:
					break;
			}
			send_filter(data);
			break;
	}
	return 0;
}
