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

