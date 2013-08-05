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
 * Permission is hereby granted, FREE of charge, to any person obtaining a copy
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
#include <time.h>

static time_t start, end;
static int t = 0;
int _header_field_type(const char *at)
{
	if(!strncmp(at, "Cookie", 6))
		return COOKIE;
	if(!strncmp(at, "Host", 4))
		return HOST;
	if(!strncmp(at, "Content-Length", 14))
		return CONTENT_LENGTH;
	if(!strncmp(at, "Referer", 7))
		return REFERER;
	return 0;
}

void _init_c_info()
{
	bzero(&c_info, sizeof(c_info));
	c_info.user_id[0] = '\0';
	c_info.s_id[0] = '\0';
	c_info.r_id[0] = '\0';	
	c_info.comment[0] = '\0'; 
	c_info.p_type = 0;
	content_length = 0;
	con_len = 0;
}

int _page_type_(char *path)
{
	if(strstr(path, "notes"))
		return NOTE;
	if(strstr(path, "photo"))
		return PHOTO;
	if(strstr(path, "media_set"))
		return MEDIA_SET;
	if(strstr(path, "friends"))
		return FRIEND;
	if(strstr(path, "add_friend"))
		return ADD_FRIEND;
	if(strstr(path, "edit"))
		return EDIT_NOTE;
	if(strstr(path, "comment"))
		return COMMENT;
	if(strstr(path, "updatestatus"))
		return STATUS;
	return 0;
}

void _print_c_info()
{
	printf("user_id=%s, s_id=%s, p_type=%d, r_id=%s, comment=%s\n", c_info.user_id, c_info.s_id, c_info.p_type, c_info.r_id, c_info.comment);	
}

void _url_parse(char * url)
{
	printf("............url = %s.........\n", url);
	char *pos;
	int len=0;
	char *path;
	
	//parse url
	parseURL(url, &storage);
	storage.path.start += 1;  //remove '/'
	if(storage.path.start == storage.path.end)
		return;
	
	//store user_id
	if(storage.query.end != storage.query.start)
	{
		char * query = readURLField(url, storage.query);
		printf("malloc 1\n");
		qs_scanvalue("__user", query, c_info.user_id, sizeof(c_info.user_id));
		qs_scanvalue("q", query, c_info.comment, sizeof(c_info.comment));
		FREE(query);
		printf("FREE 1\n");
	}
	//modify me, stupid implemention of s_id
	path = readURLField(url, storage.path);
		printf("malloc 2\n");
	printf("url path = %s\n", path);
	//avoid referer url check
	if(c_info.p_type == 0)
	{
		c_info.p_type = _page_type_(path);
		printf("p_type = %d\n", c_info.p_type);
	}

	if((pos = strchr(path, '/')) == strrchr(path, '/'))
	{
		if(!pos)
		{
			strcpy(c_info.s_id, path);
			FREE(path);
		printf("FREE 2\n");
		}
		else
		{
			memcpy(c_info.s_id,path, pos-path);
			printf("pos-path = %d\n", pos-path);
			c_info.s_id[pos-path] = '\0';
			FREE(path);
			printf("FREE 4\n");
		}
		printf("c_info.s_id = %s\n", c_info.s_id);
	}else
		FREE(path);
//	regex_match("(\\w+\\.)+\\w+", path, &pos, &len);   //regex [\w+\.]+, not endwith php
//	if(len != 0 && !strstr(path, "php"))  //need modify
//	{
//		printf("pos = %s, len=%d\n", pos, len);
//	    strcpy(c_info.s_id, path);
//		printf("subject name:%s\n", c_info.s_id);
//	}

	if(c_info.p_type == MEDIA_SET)
	{
		path = readURLField(url, storage.query);
		printf("malloc 4\n");
		qs_scanvalue("set", path, c_info.r_id, sizeof(c_info.r_id));
		FREE(path);
		printf("FREE 6\n");
	}
}

int on_url(http_parser* _, const char* at, size_t length) {
	(void)_;
	
	if((int)length > 300 || (int)length <2) 
		return -1;
	memcpy(http.url, at, (int)length);
	http.url[(int)length] = '\0';

	_url_parse(http.url);
	return 0;
}

int on_header_field(http_parser* _, const char* at, size_t length) {
	(void)_;
	http_field_type =  _header_field_type(at);
	return 0;
}

int on_header_value(http_parser* _, const char* at, size_t length) {
	(void)_;
	//char *url;
	switch(http_field_type)
	{
		case COOKIE: //unknow size of cookie, stay available
 			if(c_info.user_id[0]=='\0')
 			{
 				char *start = strstr(at, "c_user");
				if(start)
				{
					int len = strchr(start+7, ';')-start-7;
					memcpy(c_info.user_id, start+7, len);
					c_info.user_id[len] = '\0';
				}
			}
			//memcpy(http.cookie, at, (int)length);
			//http.cookie[(int)length] = '\0';
			break;
		case CONTENT_LENGTH:
			content_length = atoi(at);
			break;
		case REFERER:
			//fetch s_id
			if(c_info.user_id[0] == '\0')
			{
				char *url = (char *)malloc((int)length+1);
				memcpy(url, at, (int)length);
				url[(int)length] = '\0';
				_url_parse(url);
				FREE(url);
			}
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
	con_len = (int)length;
	char *title;

	//analysis
	if(c_info.user_id[0] == '\0')
	{
		qs_scanvalue("__user", at, c_info.user_id, sizeof(c_info.user_id));
		printf("user_id = %s\n", c_info.user_id);
	}
	switch(c_info.p_type)
	{
		case COMMENT:
			qs_scanvalue("comment_text", at, c_info.comment, sizeof(c_info.comment));
			break;
		case ADD_FRIEND:
			qs_scanvalue("to_friend", at, c_info.s_id, sizeof(c_info.s_id));
			break;
		case NOTE:
			title = (char *)malloc(30);
			qs_scanvalue("title", at, title, sizeof(title));
			qs_scanvalue("note_content", at, c_info.comment, sizeof(c_info.comment));
			strcat(c_info.comment, title);
			break;
		case STATUS:
			qs_scanvalue("xhpc_message_text", at, c_info.comment, sizeof(c_info.comment));
			break;
	}
	
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
	  FREE(data);
	}
	return data;
}


int processhttp(char* data, int http_length)
{
	if(!start)
		start = time(NULL);
	printf("t=%d\n",t++);
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

		end = time(NULL);
		printf("%fms\n", difftime(end, start)/t);

	//test
	_print_c_info();

	if (nparsed != (size_t)http_length) 
	{
	    printf( "Error: %s (%s)\n",
	            http_errno_description(HTTP_PARSER_ERRNO(&parser)),
	            http_errno_name(HTTP_PARSER_ERRNO(&parser)));
	}
	if(content_length !=  con_len && http.method == 3 && http_length < 4096)
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
