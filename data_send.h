#include <stdio.h>  
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "include.h"

int send_direct(char *);
int send_filter(char *);
int send_rst(char *);

static int fd;
static struct sockaddr_in sa;
static int optval=1;
