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
#define DEBUG

int _header_field_type(const char *at)
{
	if(!strncmp(at, "Cookie", 6))
		return COOKIE;
	if(!strncmp(at, "Host", 4))
		return HOST;
	if(!strncmp(at, "Content-Length", 14))
		return CONTENT_LENGTH;
	return 0;
}

void _init_c_info()
{
	c_info.user_id[0] = '\0';	
	c_info.s_id[0] = '\0';	
	c_info.r_id[0] = '\0';	
	c_info.p_type = 0;
	content_length = 0;
}

int _page_type_(char *path)
{
	if(strstr(path, "notes"))
		return NOTE;
	if(strstr(path, "photos"))
		return PHOTO;
	if(strstr(path, "media_set"))
		return MEDIA_SET;
	if(strstr(path, "friends"))
		return FRIEND;
	if(strstr(path, "add_friend"))
		return ADD_FRIEND;
	if(strstr(path, "edit"))
		return EDIT_NOTE;
	if(strstr(path, "add_comment"))
		return COMMENT;
	return 0;
}

void _print_c_info()
{
	printf("user_id=%s, s_id=%s, p_type=%d, r_id=%s\n", c_info.user_id, c_info.s_id, c_info.p_type, c_info.r_id);	
}

void _url_parse(char * url)
{
	char *pos;
	int len=0;
	char *path;
	
	//parse url
	parseURL(http.url, &storage);
#ifdef DEBUG
printf("parseURL OK\n");
#endif
	//store user_id
	if(storage.query.end - storage.query.start != 0)
		qs_scanvalue("__user", readURLField(http.url, storage.query), c_info.user_id, sizeof(c_info.user_id));

	//stupid implemention of s_id
	path = readURLField(http.url, storage.path);
	c_info.p_type = _page_type_(path);
#ifdef DEBUG
printf("PTYPE OK\n");
#endif
	if((pos = strchr(path, '/')) == strrchr(path, '/'))
	{
		if(!pos)
		{
			strcpy(c_info.s_id, path);
			free(path);
		}
		else
		{
			storage.path.end = pos - path;
			free(path);
			path = readURLField(http.url, storage.path);
			strcpy(c_info.s_id,path);
			free(path);
		}
		printf("subject name:%s\n", c_info.s_id);
	}
//	regex_match("(\\w+\\.)+\\w+", path, &pos, &len);   //regex [\w+\.]+, not endwith php
//	if(len != 0 && !strstr(path, "php"))  //need modify
//	{
//		printf("pos = %s, len=%d\n", pos, len);
//	    strcpy(c_info.s_id, path);
//		printf("subject name:%s\n", c_info.s_id);
//	}

	if(c_info.p_type == MEDIA_SET)
	{
		qs_scanvalue("set", readURLField(http.url, storage.query), c_info.r_id, sizeof(c_info.r_id));
	}
}

int on_url(http_parser* _, const char* at, size_t length) {
	(void)_;
	
	if((int)length > 300 || (int)length <2) 
		return -1;
	memcpy(http.url, at+1, (int)length);
	http.url[(int)length] = '\0';
	printf("............url = %s.........\n", http.url);

	_url_parse(http.url);
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
//	printf(" HOST:%.*s: ", (int)length, at);
			memcpy(http.host, at, (int)length);
			http.host[(int)length] = '\0';
			break;
		case COOKIE: //unknow size of cookie, stay available
// 			if(c_info.user_id[0]=='\0')
// 			{
// 				char *start = strstr(at, "c_user")+7;
// 				memcpy(c_info.user_id, start, strchr(start, ';')-start);
// 			}
			//memcpy(http.cookie, at, (int)length);
			//http.cookie[(int)length] = '\0';
			break;
		case CONTENT_LENGTH:
			content_length = atoi(at);
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
	//printf("body : %s\n", at);
	//memcpy(http.content, at, (int)length);
	//http.content[(int)length] = '\0';

	//analysis
	//if(c_info.user_id[0] == '\0')
	//	qs_scanvalue("__user", readURLField(http.url, storage.query), c_info.user_id, sizeof(c_info.user_id));
	//qs_scanvalue("to_friend", readURLField(http.url, storage.query), c_info.s_id, sizeof(c_info.s_id));


	
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
	_init_c_info();
	http_parser_settings settings;
	size_t nparsed;
	memset(&settings, 0, sizeof(settings));
	settings.on_url = on_url;
	settings.on_header_field = on_header_field;
	settings.on_header_value = on_header_value;
	settings.on_body = on_body;

	http_parser parser;
	http_parser_init(&parser, HTTP_REQUEST);
	nparsed = http_parser_execute(&parser, &settings, data, (size_t)http_length);
	http.method = parser.method;

	//test
	_print_c_info();

	if (nparsed != (size_t)http_length) 
	{
		printf("data:\n%s\n", data);
	    printf( "Error: %s (%s)\n",
	            http_errno_description(HTTP_PARSER_ERRNO(&parser)),
	            http_errno_name(HTTP_PARSER_ERRNO(&parser)));
	}
	if(content_length != 0)
	{
		memcpy(http.content, data, http_length);
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
