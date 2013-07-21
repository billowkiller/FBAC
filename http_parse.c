/*
 * =====================================================================================
 *
 *       Filename:  http_parse.c
 *
 *    Description:  parse http message
 *    				* from https://github.com/joyent/http-parser
 *    				*
 *
 *        Version:  1.0
 *        Created:  01/25/2013 04:19:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Billowkiller (bk), billowkiller@gmail.com
 *   Organization:
 *
 * =====================================================================================
 */

/* Based on src/http/ngx_http_parse.c from NGINX copyright Igor Sysoev
 *
 * Additional changes are licensed under the same terms as NGINX and
 * copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "http_parse.h"

int _header_field_type(const char *at)
{
	if(!strncmp(at, "Cookie", 6))
		return COOKIE;
	if(!strncmp(at, "Host", 4))
		return HOST;
	return 0;
}

void init_value()
{
	c_info.user_id[0] = '\0';	
	c_info.s_id[0] = '\0';	
	c_info.r_id[0] = '\0';	
}

int on_url(http_parser* _, const char* at, size_t length) {
	(void)_;
	char *pos;
	int len
	if((int)length >100)
		return -1;
	memcpy(http.url, at+1, (int)length);
	http.url[(int)length] = '\0';
	parseURL(http.url, &storage);
	qs_scanvalue("__user", readURLField(http.url, storage.query), c_info.user_id, sizeof(c_info.user_id));

	regex_match("(\\w+\.)+\\w+", readURLField(http.url, storage.path, &pos, &len));   //regex [\w+\.]+, not endwith php
	if(len != 0 && !strstr(readURLField(http.url, storage.path)))
	{
		c_info.s_id = readURLField(http.url, storage.path);
		printf("subject name:%s", c_info.s_id);
	}
	return 0;
}

int on_header_field(http_parser* _, const char* at, size_t length) {
	(void)_;
//	printf( "%.*s: ", (int)length, at);
//	nlines++;
//	if (nlines == MAX_HEADER_LINES) ;// error!
	http_field_type =  _header_field_type(at);
//	CURRENT_LINE->field = (char *)malloc(length+1);
//	strncpy(CURRENT_LINE->field, at, length);
//	CURRENT_LINE->field[length] = '\0';

	return 0;
}

int on_header_value(http_parser* _, const char* at, size_t length) {
	(void)_;
	switch(http_field_type)
	{
		case HOST:
			memcpy(http.host, at, (int)length);
			http.host[(int)length] = '\0';
			break;
		case COOKIE: //unknow size of cookie, stay available
			//memcpy(http.cookie, at, (int)length);
			//http.cookie[(int)length] = '\0';
			break;
	}
//	printf( "%.*s\n", (int)length, at);
//
//	CURRENT_LINE->value = (char *)malloc(length+1);
//	strncpy(CURRENT_LINE->value, at, length);
//	CURRENT_LINE->value[length] = '\0';
	
//	if(!strcmp(CURRENT_LINE->field, "Content-Type"))
//	{
//		charset_parse(CURRENT_LINE->value, contype, charset);
//	}
//	if(!strcmp(CURRENT_LINE->field, "Content-Encoding"))
//	{
//		if(!strcmp(CURRENT_LINE->value, "gzip"))
//			isGzip = 1;
//	}
	return 0;
}

int on_body(http_parser* _, const char* at, size_t length) {
	(void)_;
	printf("body : %s\n", at);
	memcpy(http.content, at, (int)length);
	http.content[(int)length] = '\0';
	
//	if(!strncmp(contype, "text/html", CONTYPE_LENGTH))
//	{
//		printf("hello, world\n");
//		//fwrite(at, 1, length, dest);
//		if(length)
//		{
//			if(isGzip)
//			{
//				char * uncomp = (char*)calloc(length*8, sizeof(char));
//				decompress(at, uncomp, length);
//
//			#ifdef HTTP_PRINT
//				printf( "Body: %.*s\n", 8*length, uncomp);
//			#endif
//
//			}else{
//				
//			#ifdef HTTP_PRINT
//				printf( "Body: %.*s\n", (int)length, at);
//			#endif
//				;
//			}
//		}
//	}
	return 0;
}

//void charset_parse(char *contype_value, char *contype, char *str)
//{
//	char * chr= strchr(contype_value, '=');
//	if(NULL != chr)
//	{
//		strcpy(str, chr+1);
//		strcpy(contype, contype_value);
//		chr = strchr(contype, ';');
//		*chr = '\0';
//	}
//	else
//	{
//		strcpy(contype, contype_value);
//		str = "utf8";
//	}
//}

char* fileRead(char *filename, long* file_length)
{
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
	  perror("fopen");
	}

	fseek(file, 0, SEEK_END);
	*file_length = ftell(file);
	if (*file_length == -1) {
	  perror("ftell");
	}
	fseek(file, 0, SEEK_SET);
	char* data = malloc(*file_length);
	if (fread(data, 1, *file_length, file) != (size_t)*file_length) {
	  fprintf(stderr, "couldn't read entire file\n");
	  free(data);
	}
	return data;
}


int processhttp(char* data, int http_length)
{
	extern GHashTable *hash_config;
	http_parser_settings settings;
	size_t nparsed;
	memset(&settings, 0, sizeof(settings));
	//settings.on_message_begin = on_message_begin;
	settings.on_url = on_url;
	settings.on_header_field = on_header_field;
	settings.on_header_value = on_header_value;
	//settings.on_headers_complete = on_headers_complete;
	settings.on_body = on_body;
	//settings.on_message_complete = on_message_complete;

	http_parser parser;
	http_parser_init(&parser, HTTP_REQUEST);
	nparsed = http_parser_execute(&parser, &settings, data, (size_t)http_length);
	http.method = parser.method;

	if (nparsed != (size_t)http_length) 
	{
	    printf( "Error: %s (%s)\n",
	            http_errno_description(HTTP_PARSER_ERRNO(&parser)),
	            http_errno_name(HTTP_PARSER_ERRNO(&parser)));
	    return FALSE;
	}

	return TRUE;
}

// int main()
// {
	
// 	long httplength = 0L;
// 	char *data;
// 	data = fileRead("b", &httplength);
// 	processhttp(data, httplength);
// }
