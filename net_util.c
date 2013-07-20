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

void _match(char* pattern,char* str,char** pos,int* len)
{
    regmatch_t pmatch;
    regex_t* preg=(regex_t*)malloc(sizeof(regex_t));
    regcomp(preg,pattern,REG_ICASE|REG_EXTENDED);
    regexec(preg,str,1,&pmatch,REG_ICASE|REG_EXTENDED);
    if(pmatch.rm_so>=0)
    {
        *len=pmatch.rm_eo-pmatch.rm_so;
        *pos=&str[pmatch.rm_so];
    }
    else
    {
        *pos=NULL;
        *len=0;
    }
}

char* _url(char *payload)
{
	char * first = strchr(payload, '/');
	int len = strchr(first, ' ') - first;
	char *url = (char *)malloc(len);
	memcpy(url, first, len);
	return url;
}

char* urldecode(char *cp)
{
    int length=0;
    int j;
    char* cpp=cp;
    while(*cpp)
    {
        if(*cpp=='%')
        {
            cpp+=2;
 
        }
        length++;
        cpp++;
    }
    length++;
    cpp=cp;
    char *p=(char*)malloc(length);
    char *pr=p;
    while(*cpp)
    {
        if(*cpp=='%')
        {
            cpp++;
            *p=(*cpp>='A'?((*cpp&0xDF)-'A')+10:(*cpp-'0'));
            *p=(*p)*16;
            cpp++;
            *p+=(*cpp>='A'?((*cpp&0xDF)-'A')+10:(*cpp-'0'));
        }
        else if(*cpp=='+')
        *p=' ';
        else *p=*cpp;
 
        p++;
        cpp++;
    }
    *p='\0';
    return pr;
}
 
 
inline char toHex(char x)
{
    return (x>9?x+55:x+'0');
}
 
char* urlencode(char *p)
{
    char *cp=p;
    int length=0;
    while(*cp)
    {
        if(isalnum(*cp));
        else if(isspace(*cp));
        else length+=2;
 
        length++;
        cp++;
    }
    length++;
    cp=p;
    char *rp=(char*)malloc(sizeof(length));
    char *r=rp;
    while(*cp)
    {
        if(isalnum(*cp))
        *rp++=*cp;
        else if(isspace(*cp))
        {
           *rp++='+';
        }
        else
        {
            *rp++='%';
            *rp++=toHex((char)(((unsigned char)*cp)>>4));//最高位为1,按无符号数计算
            *rp++=toHex((char)(((unsigned char)*cp)%16));
        }
        cp++;
    }
    *rp='\0';
    return r;
}

//according to host, not url
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

_filter_post(char *payload)
{

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
//			char *d_query = urldecode(query);		//need free
//query match
			///if(!strncmp(d_query, "吴*", 4))
				return FALSE; 
		}
	}else
	{
		char *result = strstr(payload, "\r\n\r\n")+4;
	}
	return TRUE;
	
}

int _check_blocklist(char *str)
{
	printf("*******_check_blocklist****************\n");
	printf("Sizeofhashtable:%d\n",g_hash_table_size(hash_config));
	GSList *list = g_hash_table_lookup(hash_config,"block_list");
	printf("*******list****************\n");
	GSList *iterator = NULL;
	for (iterator = list; iterator; iterator = iterator->next)
	{
		printf("*******%s\n******", (char*)iterator->data);
		if(!strstr(str, (char*)iterator->data))
			return FALSE;
	}
	return TRUE;
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
	for (iterator = list; iterator; iterator = iterator->next)
	{
		position = strstr(str, (char*)iterator->data);
		break;
	}
	return position;
}

int send_data(char *data, int flag)
{
	printf("Sizeofhashtable:%d\n",g_hash_table_size(hash_config));
	struct iphdr *iph = (struct iphdr *)data;
	struct tcphdr *tcph = (struct tcphdr *)(data + IPHL(iph));
	int http_len = IPL(iph) - IPHL(iph) - TCPHL(tcph);
	char * payload = data + TCPHL(tcph) + IPHL(iph);
	switch(flag)
	{
		case SEND_DIRECT: 
		//	send_direct(data);
			break;
		case SEND_GET:
			printf("**********in SEND_GE\n");
			processhttp(payload, http_len);
			printf("---------------processing url: %s\n", http.url);
	printf("Sizeofhashtable:%d\n",g_hash_table_size(hash_config));
			
			if(!_check_blocklist(http.url))
			{
				send_rst(data);
				printf("rst\n");
			}
			else
				;
//				send_filter(data);
			printf("*************SEND_GET FINISH***************\n");
			
			//handle url and free http, not cookie this time


		//	
		//	if(_is_forbiden(data))
		//	{
		//		printf("rst\n");
		//		send_rst(data);
		//	}
		//	else if(!_browse_filter(payload, SEND_GET)) 
		//	{	
		//		printf("rst\n");
		//		send_rst(data);
		//	}else
		//	{
		//		_replace(data);
		//		printf("yes\n");
		//		send_filter(data);
		//	}
			break;
		case SEND_POST:
			processhttp(payload, http_len);
//			send_filter(data);
		//	switch(_page_type(payload))
		//	{
		//		case STATUS:
//		//			_status_filter(payload);
		//			break;
		//		case BROWSE:
		//			break;
		//		case BLOG:
		//			break;
		//		case COMMENT:
		//			break;
		//		case FRIEND:
		//			break;
		//	}
		//	send_filter(data);
			break;
	}
	return 0;
}
