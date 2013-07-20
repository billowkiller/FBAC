/*
	Packet sniffer using libpcap library
*/
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h> // for exit()
#include <string.h> //for memset

#include <sys/socket.h>
#include <arpa/inet.h> // for inet_ntoa()
#include <net/ethernet.h>
#include <netinet/ip_icmp.h>	//Provides declarations for icmp header
#include <netinet/udp.h>	//Provides declarations for udp header
#include <netinet/tcp.h>	//Provides declarations for tcp header
#include <netinet/ip.h>	//Provides declarations for ip header
#include <glib.h>

// void process_packet(u_char *, const struct pcap_pkthdr *, const u_char *);
// void process_ip_packet(const u_char * , int);
// void print_ip_packet(const u_char * , int);
// void print_tcp_packet(const u_char *  , int );
// void print_icmp_packet(const u_char * , int );
// void PrintData (const u_char * , int);
// void deviceChose(char* devname);
void monitor();

FILE *logfile;

extern GHashTable* hash_config;

extern int isFromDest(uint32_t);
extern int isFromSrc(struct iphdr *, char *);
extern int tcp_type(struct iphdr *);
extern int content_filter(struct iphdr *);
extern int ishost(struct iphdr *, char *);
extern int send_data(char *, int);
extern int pipe_config();
