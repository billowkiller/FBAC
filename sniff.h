/*
	Packet sniffer using libpcap library
*/
#include <pcap.h>
#include <sys/socket.h>
#include <arpa/inet.h> // for inet_ntoa()
#include <net/ethernet.h>
#include <glib.h>
#include <linux/netfilter.h>		/* for NF_ACCEPT */

#include <libnetfilter_queue/libnetfilter_queue.h>
#include "include.h"

extern int store_data(const char *data);
void print_ip_header(const u_char * Buffer, int Size);
void print_tcp_packet(const u_char * Buffer, int Size);
void PrintData (const u_char * data , int Size);