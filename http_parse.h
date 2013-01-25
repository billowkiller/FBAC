#include "http.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct line {
  char *field;
  char *value;
};

#define CURRENT_LINE (&header[nlines-1])
#define MAX_HEADER_LINES 2000

static struct line header[MAX_HEADER_LINES];
static int nlines = 0;
static int contypeline = 0;

int on_message_begin(http_parser* _);
int on_headers_complete(http_parser* _);
int on_message_complete(http_parser* _);
int on_url(http_parser* _, const char* at, size_t length);
int on_header_field(http_parser* _, const char* at, size_t length);
int on_header_value(http_parser* _, const char* at, size_t length);
int on_body(http_parser* _, const char* at, size_t length);

int processhttp();
char* fileRead(char *filename, long* file_length);