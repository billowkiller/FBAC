#define DEBUG   //define debug print

#ifndef	FALSE
#define	FALSE	(0)
#endif

#ifndef	TRUE
#define	TRUE	(!FALSE)
#endif

#ifndef TCPTYPE
#define TCPTYPE
	#define FIRSTSHARK -1
	#define SECONDSHARK -2
	#define THIRDSHARK -3
	#define ACK -4
	#define GET 1
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
	#define SEND_UP 1
#endif

#ifndef WEBTYPE
#define WEBTYPE
	#define FRIEND 1
	#define STATUS 2
	#define NOTE 3
	#define COMMENT 4
	#define PHOTO 5
	#define BROWSE 6
	#define MEDIA_SET 7
	#define ADD_FRIEND 8
	#define EDIT_NOTE 9
#endif

#ifndef HTTP_HEADER_TYPE
#define HTTP_HEADER_TYPE
	#define HOST 1
	#define COOKIE 2
	#define CONTENT_LENGTH 3
	#define REFERER 4
#endif


#ifndef STRUCT
#define STRUCT
struct pseudo_hdr { /* See RFC 793 Pseudo Header */
    u_long saddr, daddr;/* source and dest address */
    u_char mbz, ptcl;	/* zero and protocol */
    u_short tcpl;	/* tcp length */
};

static int content_length;

struct HTTP{
	unsigned char method;
	int head_length;
	char url[300];
	char host[20];
	char cookie[300];
	char content[1500];
	char comment[100];
};

struct line {
  char *field;
  char *value;
};

struct connection_info{
	char user_id[20];
	char s_id[20]; //visitor view subject's page
	int p_type; /* page type */
	char r_id[50]; /* resource id */
};
#endif
