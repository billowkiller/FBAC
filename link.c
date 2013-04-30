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
Node * createNode()  
{  
     Node *node;  
     node = (Node*)malloc(sizeof(Node));  
     if(node!=NULL)  
     {  
         node->tcphdr = NULL;
         node->next = NULL;
         node->payload = NULL;
         node->datalen = 0;
     }  
     else  
     {  
        printf("Not Enough Memory!\n");   
     }
     return node;
}
int InsertNode(Node * current, struct iphdr *this_iphdr, struct tcphdr *this_tcphdr, char * payload, int datalen) 
{  
    Node *new = (Node*)malloc(sizeof(Node)); 

    new->iphdr = (struct iphdr *)malloc((unsigned int)this_iphdr->ihl*4);
    memcpy(new->iphdr, this_iphdr, (unsigned int)this_iphdr->ihl*4);

    new->tcphdr = (struct tcphdr *)malloc((unsigned int)this_tcphdr->doff*4);
    memcpy(new->tcphdr, this_tcphdr, (unsigned int)this_tcphdr->doff*4);

    current->datalen += datalen;
    current->payload = (char *)realloc(current->payload, current->datalen);
    memcpy(current->payload + current->datalen - datalen, payload, datalen);

    new->datalen = datalen;
    new->next = current->next;
    current->next = new; 
    return TRUE;  
}  
//freenextList()  
void FreeLink(Node * head)  
{   
    if(head == NULL)   
    {   
        printf("\n Empty memory! \n");   
        return;   
    }   
    Node *ptr = head;  
    Node * curr;  
    while(ptr->next != NULL)  
    {  
        curr = ptr;  
        ptr = ptr->next;  
        free(curr);    
    }  
    free(ptr);  
}


int DeleteNode(Node *node, long ack_seq)
{
    while(node->next != NULL)
    {
        if(ntohl(node->next->tcphdr->seq) < ack_seq)
        {
            Node *del = node->next;
            node->next = node->next->next;
            free(del);
        }else
            node = node->next;
    }
    return TRUE;
}