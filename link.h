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
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE 1
#endif 

typedef struct NODE  
{  
        struct NODE *next;  
        struct iphdr *iphdr;
        struct tcphdr *tcphdr;
        char * payload;
        int datalen;
}Node; 

Node * createNode();
int InsertNode(Node *, struct iphdr *, struct tcphdr *, char *, int);
void FreeLink(Node * );
int DeleteNode(Node *, long );