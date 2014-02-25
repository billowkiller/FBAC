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

typedef struct
{
    void *point;
    int len;
}vlen;


typedef struct{
    int http_len;  //origin http length
    int head_len;  //response header length
    int MSS;
    long last_seq; //last payload tcp seq
}SessionData;

//size --> seq change, allow +/-
extern int seq_register(long seq, int size);
extern int session_maintain(char **d, int from_dest);

char *CONTENTLENGTH = "Content-Length";
const char *TRANSFER_ENCODING = "Transfer-Encoding: chunked";
const char *PAGEJUMP = "<meta http-equiv=\"refresh\" content=\"0;url=http://www.baidu.com\">";
FILE *output;
SessionData *session_data;
vlen payload_cache;

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

int modify_pack(char **d, char *data, int len)
{
	//printf("%*.s", len ,data);
	int tot_head_len = IPHL(*d)+TCPHL(TCPH(*d));
	void * new_ip_pack = malloc(tot_head_len+len) ;
	memcpy(new_ip_pack, *d, tot_head_len);
	memcpy(new_ip_pack+tot_head_len, data, len);
	
    struct iphdr *iph = (struct iphdr *)new_ip_pack;
	iph->tot_len = htons(tot_head_len+len);
	*d = new_ip_pack;
}

int handle_packet(char **d)
{
    if(((struct tcphdr *)TCPH(*d))->syn)
    {
        int mss = fetch_mss(TCPH(*d));
        session_data->MSS = mss<session_data->MSS ? mss : session_data->MSS;
    }

	session_maintain(d, 1);

	if(PAYLOADL(*d)==0) return 0;

    if(!strncmp(PAYLOAD(*d), "HTTP", 4))
    {
        return handle_http_head(d);
    }
    
    int max_paylen = session_data->MSS-OPTIONL(*d);

    if(SEQ(TCPH(*d)) == session_data->last_seq)
    {
		char *add_content = "\r\n0\r\n\r\n";
        assert(max_paylen>payload_cache.len+PAYLOADL(*d)+strlen(add_content));
        char *data = (char *)malloc(payload_cache.len+PAYLOADL(*d)+strlen(add_content));
        memcpy(data, payload_cache.point, payload_cache.len);
        memcpy(data+payload_cache.len, PAYLOAD(data), PAYLOADL(*d));
        memcpy(data+payload_cache.len+PAYLOADL(*d), add_content, strlen(add_content));

		modify_pack(d, data, payload_cache.len+PAYLOADL(*d)+strlen(add_content));

        seq_register(SEQ(TCPH(*d)), payload_cache.len+strlen(add_content));
        PS("seq_register done\n");
        print_packet_info(*d, NULL);
        return 1;
    }
    else if(payload_cache.len>0)
    {
        assert(max_paylen==PAYLOADL(*d));
        void *data = malloc(max_paylen);
        memcpy(data, payload_cache.point, payload_cache.len);
        memcpy(data+payload_cache.len, PAYLOAD(*d), max_paylen-payload_cache.len);
        memcpy(payload_cache.point, PAYLOAD(*d)+max_paylen-payload_cache.len, payload_cache.len);
		modify_pack(d, data, max_paylen);
        return 1;
    }
    return 0;
}

int handle_packet2(char **d)
{
    if(((struct tcphdr *)TCPH(*d))->syn)
        session_data->MSS = fetch_mss(TCPH(*d));
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
 * fetch mss from option
 */
int fetch_mss(char *p)
{
    char *tcp = p;
    tcp += sizeof(struct tcphdr);
    assert(2 == *tcp++);
    tcp++;
    return(ntohs(*((uint16_t *)tcp)));
}
/*
 * nogzip, nochuncked
 * increase content length
 */
int handle_http_head(char **d)
{
    char *data = PAYLOAD(*d);
	char *replace_data = NULL;
    int o_paylen = PAYLOADL(*d);
	int data_len = chunck_pack(data, o_paylen, &replace_data);
    int max_paylen = session_data->MSS-OPTIONL(*d);

    if(data_len > max_paylen)
    {
        modify_pack(d, replace_data, max_paylen);
        payload_cache.len = data_len - max_paylen; 
        memcpy(payload_cache.point, replace_data+max_paylen, payload_cache.len);
        seq_register(SEQ(TCPH(*d)), max_paylen-o_paylen);
    }
    else
    {
        modify_pack(d, replace_data, data_len);
        seq_register(SEQ(TCPH(*d)), data_len-o_paylen);
    }
    session_data->last_seq= SEQ(TCPH(*d))+(1+(session_data->http_len-(o_paylen-session_data->head_len))/max_paylen)*max_paylen;

	printf("make change http head\n");

    return 1;
}

/*
 * chunked packet 
 *
 */
int chunck_pack(char *data, int data_len, char **space)
{
	//fprintf(output, "\n%.*s", data_len, data);
    char *con = strstr(data, CONTENTLENGTH);
    if(!con)
        return 0;
	
    int http_len = atoi(strlen(CONTENTLENGTH)+2+con);
	int conlen = strchr(con, '\r') - con;
	int head_len = strstr(con, "\r\n\r\n")+4-data;
	int modify_length = 2+leng(http_len)+strlen(TRANSFER_ENCODING)-conlen;
	*space = (char *)malloc(data_len+modify_length);

    session_data->http_len = http_len;
    session_data->head_len = head_len;

	int space_point = 0;
	memcpy(*space, data, con-data);
	space_point = con-data;

	memcpy(*space+space_point, TRANSFER_ENCODING, strlen(TRANSFER_ENCODING));
	space_point += strlen(TRANSFER_ENCODING);

	memcpy(*space+space_point, con+conlen, head_len-(con-data)-conlen);
	space_point += head_len-(con-data)-conlen;

    char temp[10];
    sprintf(temp, "%d", http_len);
	memcpy(*space+space_point, temp, leng(http_len));
	space_point += leng(http_len);

	if(data_len > head_len)
		memcpy(*space+space_point, data+head_len, data_len-head_len);

	//fprintf(output, "\n%.*s", data_len, *space);
	return modify_length+data_len;
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
    
    print_packet_info(pdata, 0);
    if(handle_packet((char**)&pdata))
		_recal_cksum((char**)&pdata);
	
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
    session_data = (SessionData *)malloc(sizeof(SessionData));
    session_data->MSS = 1460;

    payload_cache.point = malloc(session_data->MSS);
    payload_cache.len = 0;
    output = stderr;//fopen("visit linode", "w");

	monitor();
//    char a[]= "HTTP1.1\r\nContent-Length:800\r\nContent-Type:gzip\r\n\r\n";
//    char *s;
//    add_conlen(a, strlen(a), 200, &s);
//    fprintf(output, "%s", s);
}
