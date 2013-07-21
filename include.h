#ifndef FALSE_TRUE
#define FALSE_TRUE
	#define FALSE 0
	#define TRUE 1
#endif

#ifndef TCPTYPE
#define TCPTYPE
	#define FIRSTSHARK -1
	#define SECONDSHARK -2
	#define THIRDSHARK -3
	#define ACK 1
	#define GET 2
	#define POST 3
#endif

#ifndef HEADCAL
#define HEADCAL
	#define TCPHL(X) ((X)->doff * 4)
	#define IPHL(X) ((X)->ihl * 4)
	#define IPL(X) (ntohs((X)->tot_len))
#endif

#ifndef SENDTYPE
#define SENDTYPE
	#define SEND_DIRECT 0
	#define SEND_GET 1
	#define SEND_POST 2
#endif

#ifndef WEBTYPE
#define WEBTYPE
	#define FRIEND 1
	#define STATUS 2
	#define BLOG 3
	#define COMMENT 4
	#define BROWSE 5
#endif

#ifndef HTTP_HEADER_TYPE
#define HTTP_HEADER_TYPE
	#define HOST 1
	#define COOKIE 2
#endif


#ifndef STRUCT
#define STRUCT
struct pseudo_hdr { /* See RFC 793 Pseudo Header */
    u_long saddr, daddr;/* source and dest address */
    u_char mbz, ptcl;	/* zero and protocol */
    u_short tcpl;	/* tcp length */
};

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
#endif
