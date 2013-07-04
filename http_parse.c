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

int zcompress(char *compressdata, char *uncompressdata, int size)
{
	int ret, have, compresspoint = 0, uncompresspoint = 0;
	z_stream strm;
	unsigned char out[CHUNK];

	/* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;

    ret = inflateInit2(&strm, 16+MAX_WBITS);
    if (ret != Z_OK)
        return ret;

    /* compress until end of file */
    do
	{
		if(size - uncompresspoint< CHUNK)
		{
			strm.avail_in = size;
		}
		else 
			strm.avail_in = CHUNK;

		if(strm.avail_in == 0)
			break;

		strm.next_in = uncompressdata + uncompresspoint;
		assert(ret != Z_STREAM_ERROR);
		do
		{
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = deflate(&strm, Z_NO_FLUSH);
			switch(ret)
			{
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)deflateEnd(&strm);
					return ret;
			}
			have = CHUNK - strm.avail_out;

			memcpy(compressdata+compresspoint, out, have);
			compresspoint += have;
		}while(strm.avail_out == 0);

		uncompresspoint += CHUNK;
	}while(ret != Z_STREAM_END);
	
	(void)deflateEnd(&strm);
	return ret = Z_STREAM_END ? Z_OK : Z_DATA_ERROR;

}

int decompress(char *compressdata, char *uncompressdata, int size)
{
    int ret, have, compresspoint = 0, uncompresspoint = 0;
    z_stream strm;
	unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    
    ret = inflateInit2(&strm, 16+MAX_WBITS);
    if(ret != Z_OK)
		return ret;

	do
	{
		if(size - compresspoint< CHUNK)
		{
			strm.avail_in = size;
		}
		else 
			strm.avail_in = CHUNK;

		if(strm.avail_in == 0)
			break;

		strm.next_in = compressdata + compresspoint;
		assert(ret != Z_STREAM_ERROR);
		do
		{
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			switch(ret)
			{
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					return ret;
			}
			have = CHUNK - strm.avail_out;

			memcpy(uncompressdata+uncompresspoint, out, have);
			uncompresspoint += have;
		}while(strm.avail_out == 0);

			compresspoint += CHUNK;
	}while(ret != Z_STREAM_END);
	
	(void)inflateEnd(&strm);
	return ret = Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

FILE *dest;
int on_message_begin(http_parser* _) {
	(void)_;
	printf("\n***MESSAGE BEGIN***\n\n");
	return 0;
}

int on_headers_complete(http_parser* _) {
	(void)_;
	printf("\n***HEADERS COMPLETE***\n\n");
 	return 0;
}

int on_message_complete(http_parser* _) {
	(void)_;
	printf("contype: %s, charset: %s\n", contype, charset);
	printf("\n***MESSAGE COMPLETE***\n\n");
	return 0;
}

int on_url(http_parser* _, const char* at, size_t length) {
	(void)_;
	printf( "Url: %.*s\n", (int)length, at);
	return 0;
}

int on_header_field(http_parser* _, const char* at, size_t length) {
	(void)_;
	printf( "%.*s: ", (int)length, at);
	nlines++;
	if (nlines == MAX_HEADER_LINES) ;// error!
	CURRENT_LINE->field = (char *)malloc(length+1);
	strncpy(CURRENT_LINE->field, at, length);
	CURRENT_LINE->field[length] = '\0';

	return 0;
}

int on_header_value(http_parser* _, const char* at, size_t length) {
	(void)_;
	printf( "%.*s\n", (int)length, at);

	CURRENT_LINE->value = (char *)malloc(length+1);
	strncpy(CURRENT_LINE->value, at, length);
	CURRENT_LINE->value[length] = '\0';
	
	if(!strcmp(CURRENT_LINE->field, "Content-Type"))
	{
		charset_parse(CURRENT_LINE->value, contype, charset);
	}
	if(!strcmp(CURRENT_LINE->field, "Content-Encoding"))
	{
		if(!strcmp(CURRENT_LINE->value, "gzip"))
			isGzip = 1;
	}
	return 0;
}

int on_body(http_parser* _, const char* at, size_t length) {
	(void)_;
	printf("body length: %d\n", (int)length);
	
	if(!strncmp(contype, "text/html", CONTYPE_LENGTH))
	{
		printf("hello, world\n");
		//fwrite(at, 1, length, dest);
		if(length)
		{
			if(isGzip)
			{
				char * uncomp = (char*)calloc(length*8, sizeof(char));
				decompress(at, uncomp, length);

			#ifdef HTTP_PRINT
				printf( "Body: %.*s\n", 8*length, uncomp);
			#endif

			}else{
				
			#ifdef HTTP_PRINT
				printf( "Body: %.*s\n", (int)length, at);
			#endif
				;
			}
		}
	}
	return 0;
}

void charset_parse(char *contype_value, char *contype, char *str)
{
	char * chr= strchr(contype_value, '=');
	if(NULL != chr)
	{
		strcpy(str, chr+1);
		strcpy(contype, contype_value);
		chr = strchr(contype, ';');
		*chr = '\0';
	}
	else
	{
		strcpy(contype, contype_value);
		str = "utf8";
	}
}

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


int processhttp(FILE *file, char* data, size_t http_length)
{
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
	http_parser_init(&parser, HTTP_RESPONSE);
	nparsed = http_parser_execute(&parser, &settings, data, http_length);

	if (nparsed != (size_t)http_length) 
	{
	    fprintf(stderr,
	            "Error: %s (%s)\n",
	            http_errno_description(HTTP_PARSER_ERRNO(&parser)),
	            http_errno_name(HTTP_PARSER_ERRNO(&parser)));
	    return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

// int main()
// {
	
// 	long httplength = 0L;
// 	char *data;
// 	data = fileRead("b", &httplength);
// 	processhttp(data, httplength);
// }
