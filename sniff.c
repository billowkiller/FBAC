/*
 * =====================================================================================
 *
 *       Filename:  trafficMonitor.c
 *
 *    Description:  monitor traffic at specified device
 *    				* fliter string is defined
 *    				* loop to callback parse method
 *    				* logfile stored log.txt
 *
 *        Version:  1.0
 *        Created:  01/25/2013 03:38:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Billowkiller (bk), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "include.h"
#include <glib.h>
#include <limits.h>
#include <libnetfilter_queue/libnetfilter_queue.h>

extern int seq_register(uint32_t seq, int size);
extern int session_maintain(char **d, int from_dest);

char *CONTENTLENGTH = "Content-Length";
const char *PAGEJUMP = "<meta http-equiv=\"refresh\" content=\"0;url=http://www.baidu.com\">";
FILE *output;
int num_tcp2last = -1;

/*
 * @i 1 source-->dest
 *    0 dest-->source
 */
void print_packet_info(void *data, int i)
{
    struct tcphdr *tcph = (struct tcphdr *)TCPH(data);
    fprintf(output, "\n");
    if(i==1)
        fprintf(output, "PACKET INFO -->\n");
    else if(i==0)
        fprintf(output, "PACKET INFO <--\n");
    fprintf(output, "   |-total length       : %d\n",IPL(data));
    fprintf(output, "   |-Sequence Number    : %u\n",SEQ(TCPH(data)));
    fprintf(output, "   |-Acknowledge Number : %u\n",ntohl(tcph->ack_seq));
    fprintf(output, "   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
    fprintf(output, "   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
    fprintf(output, "   |-Payload Length: %u\n", PAYLOADL(data));
    fprintf(output, "\n");
}

int handle_packet(char **d)
{
	session_maintain(d, 1);

	if(PAYLOADL(*d)==0) return 0;

    if(!strncmp(PAYLOAD(*d), "HTTP", 4))
    {
        return handle_http_head(d, strlen(PAGEJUMP));
    }

    if(!num_tcp2last--)
    {
        print_packet_info(*d, NULL);
        char *data = (char *)malloc(IPL(*d)+strlen(PAGEJUMP));
        memcpy(data, *d, IPL(*d));
        struct iphdr *iph = (struct iphdr *)data;
        char *payload = PAYLOAD(data)+PAYLOADL(data);
        memcpy(payload, PAGEJUMP, strlen(PAGEJUMP));

        iph->tot_len = htons(IPL(data)+strlen(PAGEJUMP));
        
        *d = data;
        seq_register(SEQ(TCPH(*d)), strlen(PAGEJUMP));
        print_packet_info(*d, NULL);
        return 1;
    }
    return 0;
}

int handle_packet2(char **d)
{
	return session_maintain(d, 0);
}

//return integer figures
int leng(int a)
{
    int flag=1;
    while(a/=10)  flag++;
    return flag;
}

/*
 * nogzip, nochuncked
 * increase content length
 */
int handle_http_head(char **d, int len)
{
    char *data = PAYLOAD(*d);
    char *conlen = strstr(data, CONTENTLENGTH);
    if(!conlen)
        return 0;
    conlen += strlen(CONTENTLENGTH)+2;
    
    int data_len = IPL(data);
    int origin_len = atoi(conlen);
    int final_len = len+origin_len;
    int len_gap = leng(final_len)-leng(origin_len); 
    int con_gap = conlen-data;
    char temp[10];
    sprintf(temp, "%d\r\n", final_len);
    
    int remain = PAYLOADL(*d)-(strstr(conlen, "\r\n\r\n")+4-data);
    num_tcp2last = (origin_len-remain)/1436;

    memcpy(conlen, temp, leng(final_len));

    printf("make change http head\n");
    return 1;
}

/*
 * chunked packet 
 */
char *chunck_pack(char *data, int data_len, int len, char *space)
{
    assert(data&&len);
    char *conlen = strstr(data, CONTENTLENGTH);
    if(!conlen)
        return 0;
    conlen += strlen(CONTENTLENGTH);
    
    int origin_len = atoi(conlen+1);
    int final_len = len+origin_len;
    int len_gap = leng(final_len)-leng(origin_len); 
    int con_gap = conlen-data+1;
    char temp[10];
    sprintf(temp, "%d\r\n", final_len);

    space = malloc(data_len+len_gap+leng(final_len)+2);

    if(len_gap == 0)
    {
        memcpy(space, data, data_len);
    }
    else
    {
        memcpy(space, data, con_gap);
        memcpy(space+con_gap+leng(final_len), data+con_gap+leng(origin_len), data_len-con_gap-leng(origin_len));
    }

    memcpy(space+con_gap, temp, leng(final_len));
    memcpy(space+data_len+len_gap, temp, 2+leng(final_len));
   
    return (char *)space;
}

/*
 * handle dest->source tcp
 */
int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg,
	      struct nfq_data *nfa, void *data)
{
	u_int32_t id=0;
	struct nfqnl_msg_packet_hdr *ph;
	unsigned char *pdata = NULL;
	int pdata_len;
	
	ph = nfq_get_msg_packet_hdr(nfa);
	if (ph){
		id = ntohl(ph->packet_id);
	}
	
	pdata_len = nfq_get_payload(nfa, (char**)&pdata);
	if(pdata_len == -1){
		pdata_len = 0;
	}
    
    if(handle_packet((char**)&pdata))
		_recal_cksum((char**)&pdata);
	
    print_packet_info(pdata, 0);
	return nfq_set_verdict_mark(qh, id, NF_REPEAT, 1, IPL(pdata), pdata);
}

/*
 * handle source->dest tcp
 */
int cb2(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg,
	      struct nfq_data *nfa, void *data)
{
    u_int32_t id=0;
	struct nfqnl_msg_packet_hdr *ph;
	unsigned char *pdata = NULL;
	int pdata_len;
	
	ph = nfq_get_msg_packet_hdr(nfa);
	if (ph){
		id = ntohl(ph->packet_id);
	}
	
	pdata_len = nfq_get_payload(nfa, (char**)&pdata);
	if(pdata_len == -1){
		pdata_len = 0;
	}

    handle_packet2((char **)&pdata);

    print_packet_info(pdata, 1);
	return nfq_set_verdict_mark(qh, id, NF_REPEAT, 1, (u_int32_t)pdata_len, pdata);
}
void monitor()
{
	struct nfq_handle *h;
	struct nfq_q_handle *qh0, *qh1;
	struct nfnl_handle *nh;
	int fd;
	int rv;
	char buf[4096] __attribute__ ((aligned));

	h = nfq_open();
	if (!h) {
		printf(stderr, "error during nfq_open()\n");
		exit(1);
	}

	if (nfq_unbind_pf(h, AF_INET) < 0) {
		fprintf(stderr, "error during nfq_unbind_pf()\n");
		exit(1);
	}

	if (nfq_bind_pf(h, AF_INET) < 0) {
		fprintf(stderr, "error during nfq_bind_pf()\n");
		exit(1);
	}

    //queue num 8010
	qh0 = nfq_create_queue(h, 8010, &cb, NULL);
	if (!qh0) {
		fprintf(stderr, "error during nfq_create_queue()\n");
		exit(1);
	}

	if (nfq_set_mode(qh0, NFQNL_COPY_PACKET, 0xffff) < 0) {
		fprintf(stderr, "can't set packet_copy mode\n");
		exit(1);
	}
    
    //queue num 8011
    qh1 = nfq_create_queue(h, 8011, &cb2, NULL);
	if (!qh1) {
		fprintf(stderr, "error during nfq_create_queue()\n");
		exit(1);
	}

	if (nfq_set_mode(qh1, NFQNL_COPY_PACKET, 0xffff) < 0) {
		fprintf(stderr, "can't set packet_copy mode\n");
		exit(1);
	}

	fd = nfq_fd(h);

	for (;;) {
		if ((rv = recv(fd, buf, sizeof(buf), 0)) >= 0) {
			nfq_handle_packet(h, buf, rv);
			continue;
		}
		/* if your application is too slow to digest the packets that
		 * are sent from kernel-space, the socket buffer that we use
		 * to enqueue packets may fill up returning ENOBUFS. Depending
		 * on your application, this error may be ignored. Please, see
		 * the doxygen documentation of this library on how to improve
		 * this situation.
		 */
		if (rv < 0 && errno == ENOBUFS) {
			fprintf(output, "losing packets!\n");
			continue;
		}
		perror("recv failed");
		break;
	}

	fprintf(output, "unbinding from queue 0\n");
	nfq_destroy_queue(qh0);
	nfq_destroy_queue(qh1);

#ifdef INSANE
	/* normally, applications SHOULD NOT issue this command, since
	 * it detaches other programs/sockets from AF_INET, too ! */
	fprintf(output, "unbinding from AF_INET\n");
	nfq_unbind_pf(h, AF_INET);
#endif

	fprintf(output, "closing library handle\n");
	nfq_close(h);

	exit(0);
}

int main()
{
    output = stderr;//fopen("visit linode", "w");
	monitor();
//    char a[]= "HTTP1.1\r\nContent-Length:800\r\nContent-Type:gzip\r\n\r\n";
//    char *s;
//    add_conlen(a, strlen(a), 200, &s);
//    fprintf(output, "%s", s);
}
