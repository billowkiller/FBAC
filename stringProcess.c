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

void regex_match(char* pattern,char* str,char** pos,int* len)
{
    regmatch_t pmatch;
    regex_t* preg=(regex_t*)malloc(sizeof(regex_t));
    regcomp(preg,pattern,REG_ICASE|REG_EXTENDED);
    regexec(preg,str,1,&pmatch,REG_ICASE|REG_EXTENDED);
    if(pmatch.rm_so>=0&&pmatch.rm_so<strlen(str))
    {
        *len=pmatch.rm_eo-pmatch.rm_so;
        *pos=&str[pmatch.rm_so];
    }
    else
    {
        *pos=NULL;
        *len=0;
    }
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

char* urldecode(char *cp)
{
    int length=0;
    int j;
    char* cpp=cp;
    while(*cpp)
    {
        if(*cpp=='%')
        {
            cpp+=2;
 
        }
        length++;
        cpp++;
    }
    length++;
    cpp=cp;
    char *p=(char*)malloc(length);
    char *pr=p;
    while(*cpp)
    {
        if(*cpp=='%')
        {
            cpp++;
            *p=(*cpp>='A'?((*cpp&0xDF)-'A')+10:(*cpp-'0'));
            *p=(*p)*16;
            cpp++;
            *p+=(*cpp>='A'?((*cpp&0xDF)-'A')+10:(*cpp-'0'));
        }
        else if(*cpp=='+')
        *p=' ';
        else *p=*cpp;
 
        p++;
        cpp++;
    }
    *p='\0';
    return pr;
}
 
 
inline char toHex(char x)
{
    return (x>9?x+55:x+'0');
}
 
char* urlencode(char *p)
{
    char *cp=p;
    int length=0;
    while(*cp)
    {
        if(isalnum(*cp));
        else if(isspace(*cp));
        else length+=2;
 
        length++;
        cp++;
    }
    length++;
    cp=p;
    char *rp=(char*)malloc(sizeof(length));
    char *r=rp;
    while(*cp)
    {
        if(isalnum(*cp))
        *rp++=*cp;
        else if(isspace(*cp))
        {
           *rp++='+';
        }
        else
        {
            *rp++='%';
            *rp++=toHex((char)(((unsigned char)*cp)>>4));
            *rp++=toHex((char)(((unsigned char)*cp)%16));
        }
        cp++;
    }
    *rp='\0';
    return r;
}
// int main()
// {
//     char a[] = "GET /search?q=billowkiller HTTP/1.1\r\nHost: www.douban.com\r\nUser-Agent: Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:20.0) Gecko/20100101 Firefox/20.0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nCookie: bid=\"ThlqzbkYjrI\"; __utma=30149280.942647323.1368419994.1368440128.1368443953.3; __utmc=30149280; __utmz=30149280.1368419994.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); __utmb=30149280.5.10.1368443953\nConnection: keep-alive\nCache-Control: max-age=0";
    
//     printf("%d\n", strcmp("/", "/search?q=nihao"));
// }
