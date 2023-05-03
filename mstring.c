#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#include "mlogger.h"

#define SPLIT_SIZE_INITIAL 8
#define SPLIT_GROW_FACTOR 2

char **msplit(char *input_string, const char * const separators) {
    assert( input_string != NULL );
    const char * const default_sep = " \t\n\r";
    // Copy the input string.
    size_t array_length = SPLIT_SIZE_INITIAL;
    size_t nwords = 0;
    char **split_array = NULL;

    char *seps = NULL;

    logmsg(MLOG_DEBUG, "msplit input string: '%s'", input_string);

    if (separators == NULL) {
        seps = (char *)malloc(sizeof(char)*5);
        assert( seps != NULL );
        strcpy(seps, default_sep);
    } else {
        seps = (char *)malloc(sizeof(char)*strlen(separators));
        assert( seps != NULL );
        strcpy(seps, separators);
    }

    int inword = 0;
    int word_length = 0;
    for (int i = 0; i < strlen(input_string); i++) {
        for (int j = 0; j < strlen(seps); j++) {
            if (input_string[i] == seps[j]) {
                if (inword) {
                    // Then we reached the end of a word.
                    inword = 0;
                    // malloc a new string in the array and copy it over.
                    split_array[nwords] = (char *)malloc(sizeof(char)*word_length+1);
                    assert( split_array[nwords] != NULL );
                    assert( i >= word_length );
                    strncpy(split_array[nwords],
                            input_string+i-word_length,
                            word_length);
                }
                continue;
            } else if (! inword) {
                inword = 1;
                word_length = 1;
                nwords++;
                if (nwords+1 > array_length) {
                    array_length *= SPLIT_GROW_FACTOR;
                    split_array = (char **)realloc(split_array,
                                                   sizeof(char *)*array_length);
                    assert( split_array != NULL );
                }
            } else {
                // If inword, then count the size of the word.
                if (inword) {
                    word_length++;
                }
            }
        }
    }
    split_array[nwords+1] = NULL;
    return split_array;
}

void
free_msplit(char **split_string) {
    int i = 0;
    while (split_string[i] != NULL) {
        free(split_string[i]);
        i++;
    }
    free(split_string);
}

#include "mstring.h"
