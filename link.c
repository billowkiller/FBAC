/*
 * =====================================================================================
 *
 *       Filename:  link.c
 *
 *    Description:  TCPLink List
 *                  -   first node is setinal
 *                  -   first node's payload and datalen is all 
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
#include "link.h"
//createNode()  
TCPLink create_TCPLink()
{  
     Node *node;  
     node = (Node*)malloc(sizeof(Node));  
     if(node!=NULL)  
     {  
     	 node->iphdr = NULL;
         node->tcphdr = NULL;
         node->next = NULL;
         node->payload = NULL;
		 node->reserve = NULL;
         node->datalen = 0;
         node->state = UNCHECK;
     }  
     else  
     {  
        printf("Not Enough Memory!\n");   
     }
     return node;
}

int is_after_syn(TCPLink link)
{
	if(link && !(link->next))
		return TRUE;
	else
		return FALSE;
}

int is_http_header(Node * curr)
{
	if(curr->payload && !strncmp(curr->payload, "HTTP", 4))
		return TRUE;
	else
		return FALSE;
}

int is_fin(Node *node)
{
	return (int)(node->tcphdr->fin);
}

void print_tcpheader(struct tcphdr *tcph)
{
	 printf("\n");
        printf("TCP Header\n");
        printf("   |-Source Port      : %u\n",ntohs(tcph->source));
        printf("   |-Destination Port : %u\n",ntohs(tcph->dest));
        printf("   |-Sequence Number    : %u\n",ntohl(tcph->seq));
        printf("   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
        printf("   |-Push Flag            : %d\n",(unsigned int)tcph->psh);
        printf("   |-Reset Flag           : %d\n",(unsigned int)tcph->rst);
        printf("   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn);
        printf("   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
        printf("\n");
}

void print_link(TCPLink link)
{
	Node * curr = link;
	if(link == NULL)   
    {     
        return;   
    }
    while(curr)
	{
		PD(SEQ(curr->tcphdr));
		PD(curr->state);
// 		PD(curr->datalen);
// 		if(curr->datalen >1)
// 			PS(curr->payload);
		curr = curr->next;
	}
}

//state is check before last fin
int check_complete_link(TCPLink link)
{
	if(link == NULL || link->state == CHECK || link->state == UNCHECK)
		return FALSE;
	
	Node * curr = link;
    while(curr)
	{
		if(curr->state == UNCHECK)
			return FALSE;
		curr = curr->next;
	}
	printf("check_complete_link is true!\n");
	//PD(length);
	return TRUE;

}

void _replace_node(Node * curr, Node * new)
{
	if(curr == new)
		return;
	
	free(curr->iphdr);
	free(curr->tcphdr);
	if(curr->payload)
		free(curr->payload);
	
	memcpy(curr, new, sizeof(Node));
	free(new);
}

/* strange logic
 * 
 * need to keep the link address
 * cant delete the first node until last
 * using replace
 */
int *get_next_data(TCPLink link, int self, void (*func)(Node *node))
{
	
	//duplicate ack
	if(self && link->payload)
	{
		func(link);
	}
	
	Node * curr = link;
	Node * ncurr = curr->next;
	
	if(ncurr && ncurr->state == CHECK)
	{
		curr = ncurr;
		ncurr = curr->next;
	}
	else
		return FALSE;
	
	while(ncurr && ncurr->state == CHECK)
	{
		if(curr->datalen > 1)
		{
			func(curr);
		}
		FreeNode(curr);
		curr = ncurr;
		ncurr = curr->next;
	}
	_replace_node(link, curr);
	
	if(is_fin(link))
	{	
		if(link->datalen > 1)
		{
			func(link);
		}
		return TRUE;
	}
	else
		return FALSE;
}

/* TRUE means the half connection is completed */
int insert_packet(TCPLink link, struct iphdr *this_iphdr, struct tcphdr *this_tcphdr, char * payload, int datalen, void * reserve, void (*func)(Node *node))
{  
	/* refuse seq > fin.seq which means finack */
	if(link->state != CHECK && link->state != UNCHECK && SEQ(this_tcphdr)>link->state)
		return FALSE;
	
	//print_tcpheader(this_tcphdr);
    Node * curr = link;
    	 
	Node *new = NEW_NODE();
	new->iphdr = (struct iphdr *)malloc((unsigned int)this_iphdr->ihl*4);
    memcpy(new->iphdr, this_iphdr, (unsigned int)this_iphdr->ihl*4);

    new->tcphdr = (struct tcphdr *)malloc((unsigned int)this_tcphdr->doff*4);
    memcpy(new->tcphdr, this_tcphdr, (unsigned int)this_tcphdr->doff*4);
	
	new->reserve = reserve;
	
	if(datalen)
	{
		new->payload = (char *)malloc(datalen);
		memcpy(new->payload, payload, datalen);
	
		new->datalen = datalen;
	}
	else
		new->datalen = 1;  //seq consume 
	
	/* first node */
    if(!(curr->iphdr))
    {
    	new->state = CHECK;
		
    	memcpy(link, new, sizeof(Node));
		free(new);
    	return FALSE;
    }
    
    PD(SEQ(link->tcphdr));
    /* normal package comes */
	long seq = SEQ(this_tcphdr);
	Node *ncurr = curr->next;
	
	while(TRUE)
	{
		/* tcp duplication */
		if(seq == SEQ(curr->tcphdr))
		{
			/* ack with additional information */
			if(datalen > curr->datalen)
			{
				ncurr = curr->next;
				_replace_node(curr, new);
				curr->next = ncurr;
				
				if(ncurr && seq + datalen == SEQ(ncurr->tcphdr))
					ncurr->state = CHECK;
				
				return get_next_data(link, 1, func);
			}
			return FALSE;
			
		}
		
		/* find the right place to insert */
		if(ncurr && SEQ(ncurr->tcphdr) <= seq)
		{
			curr = curr->next;
			ncurr = curr->next;
		}
		else 
			break;
	}
		
	new->next = curr->next;
    curr->next = new;

	/*
	 * curr->new->ncurr
	 * seq + datalen = nextseq
	 */
	if(SEQ(curr->tcphdr)+curr->datalen == seq)
		new->state = CHECK;
	if(ncurr && seq + new->datalen == SEQ(ncurr->tcphdr))
		ncurr->state = CHECK;
	
	//print_link(link);
	return get_next_data(link, 0, func);
}

//freenextList()  
void FreeLink(TCPLink head)  
{   
    if(head == NULL)   
    {     
        return;   
    }   
    Node *ptr = head;  
    Node * curr;  
    while(ptr->next != NULL)  
    {  
        curr = ptr;  
        ptr = ptr->next;  
        FreeNode(curr);    
    }  
    FreeNode(ptr);  
}


int delete_ack_node(Node *node, long ack_seq)
{
    while(node->next != NULL)
    {
        if(ntohl(node->next->tcphdr->seq) < ack_seq)
        {
            Node *del = node->next;
            node->next = node->next->next;
            FreeNode(del);
        }else
            node = node->next;
    }
    return TRUE;
}

int FreeNode(Node *node)
{
	printf("delete seq %d\n", SEQ(node->tcphdr));
	free(node->iphdr);
	free(node->tcphdr);
	if(node->payload) free(node->payload);
	free(node);
}
