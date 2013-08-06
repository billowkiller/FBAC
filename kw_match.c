/*
 * =====================================================================================
 *
 *       Filename:  kw_match.c
 *
 *    Description:  check whether string contains keywords
 *
 *        Version:  1.0
 *        Created:  08/04/2013 08:46:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  outao
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <string.h>
#include <glib.h>

#define MAXWORD 30

static GSList *g_kw_list = NULL;

void read_kw_file()
{
	FILE *file = fopen("keywords", "r");
	while(TRUE)
	{
		char *word = (char *)malloc(MAXWORD);
		int n = fscanf(file, "%s", word);
		if(n > 0)
			add_kw_list(word);
		else
			break;
	}
	fclose(file);
}

void add_kw_list(char * kw)
{
	g_kw_list = g_list_append(g_kw_list, kw);
}

char* _fileRead(char *filename)
{
	long file_length;
	FILE* file = fopen(filename, "r");
	if (file == NULL) {
	  perror("fopen");
	}

	fseek(file, 0, SEEK_END);
	file_length = ftell(file);
	if (file_length == -1) {
	  perror("ftell");
	}
	fseek(file, 0, SEEK_SET);
	char* data = malloc(file_length);
	if (fread(data, 1, file_length, file) != (size_t)file_length) {
	  fprintf(stderr, "couldn't read entire file\n");
	  free(data);
	}
	return data;
}

int kw_match(char *str)
{
	if(str[0]=='\0')
		return 0;
	printf("str = %s\n", str);
	GSList *iterator = NULL;
	for (iterator = g_kw_list; iterator; iterator = iterator->next)
		 if(strstr(str, (char*)iterator->data))
		 {
			 printf("data = %s\n", (char *)iterator->data);
			 return 1;
		 }
	return 0;
}
