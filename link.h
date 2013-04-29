/*
 * =====================================================================================
 *
 *       Filename:  link.h
 *
 *    Description:  TCPLink List
 *					-	first node is setinal
 *					-	first node's payload and datalen is all
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

#include <stdio.h>  
#include <stdlib.h>  
#include <netinet/tcp.h>
#define FALSE 0  
#define TRUE  1  

typedef struct NODE  
{  
        struct NODE *next;  
        struct tcphdr *tcphdr;
        char * payload;
        int datalen;
}Node; 

Node * createNode();
int InsertNode(Node * current, struct tcphdr *this_tcphdr, char * payload, int datalen);
void FreeLink(Node * head);
int DeleteNode(Node *node, long seq);