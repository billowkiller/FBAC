#ifndef _string_process_h
#define _string_process_h

#include <string.h>

const char *kmp_search(const char *text, const char *pattern);
void regex_match(char*, char*, char**, int*);

#endif
