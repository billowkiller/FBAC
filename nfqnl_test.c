#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <linux/netfilter.h>		/* for NF_ACCEPT */
#include <errno.h>

#include <libnetfilter_queue/libnetfilter_queue.h>


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
	
	struct iphdr *iph = (struct iphdr *)pdata;
	struct tcphdr *tcph= (struct tcphdr*)((u_int8_t*)iph+ (iph->ihl<<2));
	int http_len = IPL(iph) - IPHL(iph) - TCPHL(tcph);
    char * payload = pdata + TCPHL(tcph) + IPHL(iph);
	printf("pdata_len = %d\n", pdata_len);
	printf("http_len = %d\n", http_len);
	if(http_len)
		printf("%s\n", payload);
	
	return nfq_set_verdict_mark(qh, id, NF_REPEAT, 1, (u_int32_t)pdata_len, pdata);
}

int main(int argc, char **argv)
{
	struct nfq_handle *h;
	struct nfq_q_handle *qh;
	struct nfnl_handle *nh;
	int fd;
	int rv;
	char buf[4096] __attribute__ ((aligned));

	printf("opening library handle\n");
	h = nfq_open();
	if (!h) {
		fprintf(stderr, "error during nfq_open()\n");
		exit(1);
	}

	printf("unbinding existing nf_queue handler for AF_INET (if any)\n");
	if (nfq_unbind_pf(h, AF_INET) < 0) {
		fprintf(stderr, "error during nfq_unbind_pf()\n");
		exit(1);
	}

	printf("binding nfnetlink_queue as nf_queue handler for AF_INET\n");
	if (nfq_bind_pf(h, AF_INET) < 0) {
		fprintf(stderr, "error during nfq_bind_pf()\n");
		exit(1);
	}
	
	int qid = 0;
	if(argc == 2){
		qid = atoi(argv[1]);
	}

	printf("binding this socket to queue '%d'\n", qid);
	qh = nfq_create_queue(h,  qid, &cb, NULL);
	if (!qh) {
		fprintf(stderr, "error during nfq_create_queue()\n");
		exit(1);
	}

	printf("setting copy_packet mode\n");
	if (nfq_set_mode(qh, NFQNL_COPY_PACKET, 0xffff) < 0) {
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
			printf("losing packets!\n");
			continue;
		}
		perror("recv failed");
		break;
	}

	printf("unbinding from queue 0\n");
	nfq_destroy_queue(qh);

#ifdef INSANE
	/* normally, applications SHOULD NOT issue this command, since
	 * it detaches other programs/sockets from AF_INET, too ! */
	printf("unbinding from AF_INET\n");
	nfq_unbind_pf(h, AF_INET);
#endif

	printf("closing library handle\n");
	nfq_close(h);

	exit(0);
}
