/*
	Packet sniffer using libpcap library
*/
#include <pcap.h>
#include <sys/socket.h>
#include <arpa/inet.h> // for inet_ntoa()
#include <net/ethernet.h>
#include <glib.h>
#include <sqlite3.h>

#include "include.h"

// void process_packet(u_char *, const struct pcap_pkthdr *, const u_char *);
// void process_ip_packet(const u_char * , int);
// void print_ip_packet(const u_char * , int);
// void print_tcp_packet(const u_char *  , int );
// void print_icmp_packet(const u_char * , int );
// void PrintData (const u_char * , int);
// void deviceChose(char* devname);

FILE *logfile;
sqlite3 *db = NULL;

extern GHashTable* hash_config;

extern int isFromDest(uint32_t);
extern int isFromSrc(struct iphdr *, char *);
extern int tcp_type(struct iphdr *);
extern int content_filter(struct iphdr *);
extern int ishost(struct iphdr *, char *);
extern int send_data(char *, int);
extern int pipe_config();
extern int store_data(const char *data);
extern int get_data(u_int32_t src, u_int32_t dst, u_int16_t sport, u_int16_t dport);
