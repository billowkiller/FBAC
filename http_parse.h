#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "http.h"
#include "urlparser.h"

#include "include.h"
#include "qs_parse.h"

#define CURRENT_LINE (&header[nlines-1])
#define MAX_HEADER_LINES 2000
#define CONTYPE_LENGTH 9

static struct line header[MAX_HEADER_LINES];
static int nlines = 0;
static int isGzip = 0;
static int http_field_type = 0;
static char charset[10];
static char contype[30];
static URL storage;

struct HTTP http;
struct connection_info c_info;
int processhttp(char*, int);

extern void regex_match(char*, char*, char**, int*);
//#define HTTP_PRINT
