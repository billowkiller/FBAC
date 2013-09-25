#include "link.h"
#include <glib.h>
#include <pthread.h>

typedef struct {
  u_int32_t src;		/* Source IP address */
  u_int32_t dst;		/* Destination IP address */
  u_int16_t sport;		/* Source port number */
  u_int16_t dport;		/* Destination port number */
} flow_t; 

#define HASH_SIZE           1009  /* prime number near 1000 */

#define HASH_FLOW(flow) ( \
( (flow.sport & 0xff) | ((flow.dport & 0xff) << 8) | \
  ((flow.src & 0xff) << 16) | ((flow.dst & 0xff) << 24) \
) )

static int ifile = 0;
char *filen[12] = {"0","1","2","3","4","5","6","7","8","9","10","11"};

GSList *g_list = NULL;
extern FILE *file;
int get_data(void *arg);

int tcp_type(struct tcphdr *tcph)
{
	if(tcph->syn & ~(tcph->ack) & ~(tcph->rst))
		return FIRSTSHARK;
	
	if(tcph->syn & tcph->ack)
		return SECONDSHARK;
	
	if(tcph->fin)
		return FIN;
// 	
// 	if((~(tcph->syn) & tcph->ack)
// 		&& 40 == ntohs(iph->tot_len))
// 		return ACK;

	return 0;
}

TCPLink g_list_lookup(flow_t flow)
{
	GSList *iterator = NULL;
	TCPLink tcp_link = NULL;
	for (iterator = g_list; iterator; iterator = iterator->next) {
		tcp_link = (TCPLink)iterator->data;
		if(ntohl(tcp_link->iphdr->saddr) == flow.src
			&& ntohl(tcp_link->iphdr->daddr) == flow.dst
			&& ntohs(tcp_link->tcphdr->source) == flow.sport
			&& ntohs(tcp_link->tcphdr->dest) == flow.dport)
			break;
	}
	return tcp_link;
}

void deal_data(Node *node)
{
	char *payload;
	int length;
	if(node->reserve)
	{
		payload = (char *)(node->reserve);
		length = node->datalen - (payload - node->payload);
	}
	
// 	FILE *tempfile = fopen(filen[ifile++], "w");
// 	fwrite(payload, length, 1, tempfile);
// 	fclose(tempfile);
	printf( "%.*s\n", length, payload);
	/*
	 * unzip data in another thread 
	 * return the string and then check validation
	 * resent or not
	 */
	int i = 1;
	if(i)
		send_direct((char *)(node->iphdr));
	else
		send_rst((char *)(node->iphdr));
}

/*	return
 *  @ -1 not http header
 *  @ 0 not target page, like picture
 *  @ 1 target page
 */ 
int http_parse(const char *data, char **split)
{
	if(strncmp(data, "HTTP", 4))
		return -1;
	
	char * type = strstr(data, "Content-Type") + 13;
	if(strncmp(type, "text/html", 9))
		return 0;
	
	*split = strstr(type, "\r\n\r\n") + 4;
	
	return 1;
		
}

int store_data(const char *data)
{
	struct iphdr *iph = (struct iphdr *)data;
	struct tcphdr *tcph = (struct tcphdr *)(data + IPHL(iph));
	flow_t this_flow;
	TCPLink tcp_link;
	int datalen;
	char * payload, reserve = NULL;
	
	/* fill in the flow_t structure with info that identifies this flow */
	this_flow.src = ntohl(iph->saddr);
	this_flow.dst = ntohl(iph->daddr);
	this_flow.sport = ntohs(tcph->source);
	this_flow.dport = ntohs(tcph->dest);
	
// 	print_tcpheader(tcph);
// 	return 0;
	PD(SEQ(tcph));
	switch(tcp_type(tcph))
	{
		case SECONDSHARK: //start a new process
			printf("SECONDSHARK\n");
			tcp_link = g_list_lookup(this_flow);
			if(tcp_link)
			{
				FreeLink(tcp_link);
				g_list = g_slist_remove(g_list, tcp_link);
			}
			tcp_link = create_TCPLink();
			insert_packet(tcp_link, iph, tcph, NULL, 0, (void *)reserve, deal_data);
			g_list = g_list_append(g_list, tcp_link);
			
			send_direct(data);
			break;
		case FIN:  /* fin can carry data. */
			printf("FIN\n");
		default:
			datalen = IPL(iph) - IPHL(iph) - TCPHL(tcph);
			//debug me, if ack
			payload = data + TCPHL(tcph) + IPHL(iph);
			tcp_link = g_list_lookup(this_flow);
			
			if(!tcp_link)
			{
				send_direct(data);
				return;
			}
			
			if(datalen && !http_parse(data, &reserve))
			{
				send_direct(data);
				printf("remove list\n");
				FreeLink(tcp_link);
				g_list = g_slist_remove(g_list, tcp_link);
				return;		
			}
			
			if(insert_packet(tcp_link, iph, tcph, payload, datalen, (void *)reserve, deal_data))
			{
				printf("remove list\n");
				FreeLink(tcp_link);
				g_list = g_slist_remove(g_list, tcp_link);
			}
	}
}
