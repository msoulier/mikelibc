#ifndef MSTRING_H
#define MSTRING_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SEPARATOR 1024

/*
 * Take a string a split on whitespace, returning an array of strings.
 * This array is dynamically allocated. After you are done with it,
 * call free_msplit below. Note that the strings AND the array are both
 * NULL terminated. The separators string contains one or more chars that
 * are used to split on. Concurrent occurrances of any of them will be used
 * as a combined delimiter. If separators is NULL, it defaults to 
 * " \t\n\r". This function does not alter the input string. The input string
 * is trusted to be properly null terminated. Be certain of your input.
 * The separators have a maximum size of MAX_SEPARATOR (1024).
 */
char **msplit(char *input_string, const char * const separators);

/*
 * Use this to free the dynamically allocated array of strings from
 * msplit.
 */
void
free_msplit(char **split_string);

#ifdef __cplusplus
}
#endif

#endif
