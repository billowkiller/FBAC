/*
 * =====================================================================================
 *
 *       Filename:  stringProcess.c
 *
 *    Description:  FBAC StringProcess
 *    				* pattern match
 *
 *        Version:  1.0
 *        Created:  01/25/2013 03:08:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Billowkiller (bk), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "stringProcess.h"

void trim(char * str)
{
	int i=0, j=0;
	while(*(str+i) == ' ')	i++;
	while(*(str+i+(++j)) != ' ');
	char trim[j+1];
	strncpy(trim, str+i, j);
	trim[j] = '\0';
	strcpy(str, trim);
}

const char *kmp_search(const char *text, const char *pattern)
{
    int *T;
    int i, j;
    const char *result = NULL;
 
    if (pattern[0] == '\0')
        return text;
 
    /* Construct the lookup table */
    T = (int*) malloc((strlen(pattern)+1) * sizeof(int) );
    T[0] = -1;
    for (i=0; pattern[i] != '\0'; i++) {
        T[i+1] = T[i] + 1;
        while (T[i+1] > 0 && pattern[i] != pattern[T[i+1]-1])
            T[i+1] = T[T[i+1]-1] + 1;
    }
 
    /* Perform the search */
    for (i=j=0; text[i] != '\0'; ) {
        if (j < 0 || text[i] == pattern[j]) {
            ++i, ++j;
            if (pattern[j] == '\0') {
                result = text+i-j;
                break;
            }
        }
        else j = T[j];
    }
 
    free(T);
    return result;
}

// int main()
// {
//     char a[] = "GET /search?q=billowkiller HTTP/1.1\r\nHost: www.douban.com\r\nUser-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:20.0) Gecko/20100101 Firefox/20.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nCookie: bid=\"ThlqzbkYjrI\"; __utma=30149280.942647323.1368419994.1368440128.1368443953.3; __utmc=30149280; __utmz=30149280.1368419994.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); __utmb=30149280.5.10.1368443953\nConnection: keep-alive\nCache-Control: max-age=0";
    
//     printf("%d\n", strcmp("/", "/search?q=nihao"));
// }
