#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef TCPTYPE
	#define FIRSTSHARK -1
	#define SECONDSHARK -2
	#define THIRDSHARK -3
	#define ACK 1
	#define GET 2
	#define POST 3
#endif

#ifndef HEADCAL
	#define TCPHL(X) ((X)->doff * 4)
	#define IPHL(X) ((X)->ihl * 4)
	#define IPL(X) (ntohs((X)->tot_len))
#endif

#define SEND_DIRECT 0
#define SEND_GET 1
#define SEND_POST 2

#ifndef WEBTYPE
	#define FRIEND 1
	#define STATUS 2
	#define BLOG 3
	#define COMMENT 4
	#define BROWSE 5
#endif

struct HTTP{
	char url[100];
	char host[20];
	char cookie[300];
	char content[200];
};

struct line {
  char *field;
  char *value;
};

#ifndef HTTP_HEADER_TYPE
	#define HOST 1
	#define COOKIE 2
#endif

struct pseudo_hdr { /* See RFC 793 Pseudo Header */
    u_long saddr, daddr;/* source and dest address */
    u_char mbz, ptcl;	/* zero and protocol */
    u_short tcpl;	/* tcp length */
};


#define PSEUDO_SIZE (sizeof(struct pseudo_hdr))

#ifndef SENDTYPE
	#define DIRECT 1
	#define FILTER 2
#endif