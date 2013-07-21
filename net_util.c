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
    if(pmatch.rm_so>=0&&pmatch.rm_so<strlen(str))
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
int _page_type(char *host)
{
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
		_match((char*)iterator->data, str, &position, &len);
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
	printf("%s\n", payload);
	switch(flag)
	{
		case SEND_DIRECT: 
			send_direct(data);
			break;
		case SEND_GET:
			printf("**********in SEND_GE\n");
			processhttp(payload, http_len);
			printf("url: %s\n", http.url);
			
			if(_check_blocklist(http.url))
			{
				send_rst(data);
				printf("rst\n");
			}
			else if(_check_strlist(http.url))
				send_filter(data);
			else 
				send_filter(data);
			printf("*************SEND_GET FINISH***************\n");
			
			//handle url and free http, not cookie this time
			break;
		case SEND_POST:
			processhttp(payload, http_len);

			printf(".......host:%s\n", http.host);
			switch(_page_type(http.host))
			{
				case STATUS:
					printf(".....status content:%s\n", http.content);
					_check_strlist(http.content);
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
