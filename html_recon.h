#include <nids.h>
#include <stdio.h>

#define int_ntoa(x)	inet_ntoa(*((struct in_addr *)&x))

struct html
{
	char *data;
	int length;
};

void store_html(char *, int);