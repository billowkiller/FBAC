#include "include.h"

#define UNCHECK 0
#define CHECK 1

#define NEW_NODE() create_TCPLink()

typedef struct NODE  
{  
        struct NODE *next;  
        struct iphdr *iphdr;
        struct tcphdr *tcphdr;
        char * payload;
        int datalen;
		void * reserve;
        int state; //CHECK, UNCHECK
}Node;
typedef Node * TCPLink;

TCPLink create_TCPLink();
int insert_packet(TCPLink link, struct iphdr *this_iphdr, 
			  struct tcphdr *this_tcphdr, char * payload, int datalen);

void FreeLink(TCPLink head);
void print_tcpheader(struct tcphdr *tcph);
int is_fin(Node *node);