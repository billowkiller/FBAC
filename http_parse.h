#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "urlparser.h"

#include "include.h"
#include "qs_parse.h"

#define CURRENT_LINE (&header[nlines-1])
#define MAX_HEADER_LINES 2000

#ifndef HTTP_HEADER_TYPE
#define HTTP_HEADER_TYPE
	#define HOST 1
	#define COOKIE 2
	#define CONTENT_LENGTH 3
	#define REFERER 4
#endif

struct line {
  char *field;
  char *value;
};

static int nlines = 0;
static int isGzip = 0;
static int http_field_type = 0;
static char charset[10];
static char contype[30];
static URL storage;
static int con_len=0;  //content length
static int content_length;  //http header field

struct HTTP http;
struct connection_info c_info;
int processhttp(char*, int);

extern void regex_match(char*, char*, char**, int*);
extern char* urlencode(char *);
extern char* urldecode(char *);
