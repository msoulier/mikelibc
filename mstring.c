#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>

#include "mdebug.h"

#define SPLIT_SIZE_INITIAL 8
#define SPLIT_GROW_FACTOR 2

char **msplit(char *input_string, const char * const separators) {
    mdebugf("in msplit\n");
    assert( input_string != NULL );
    const char * const default_sep = " \t\n\r";
    // Copy the input string.
    size_t array_length = SPLIT_SIZE_INITIAL;
    size_t nwords = 0;
    mdebugf("malloc of split_array size %d\n", sizeof(char *)*SPLIT_SIZE_INITIAL);
    char **split_array = (char **)malloc(sizeof(char *)*SPLIT_SIZE_INITIAL);
    assert( split_array != NULL );

    char *seps = NULL;

    mdebugf("msplit input string: '%s'\n", input_string);

    if (separators == NULL) {
        mdebugf("malloc of seps size %d\n", sizeof(char)*5);
        seps = (char *)malloc(sizeof(char)*5);
        assert( seps != NULL );
        strcpy(seps, default_sep);
        mdebugf("using default separators\n");
    } else {
        mdebugf("malloc of seps size %d\n", sizeof(char)*strlen(separators));
        seps = (char *)malloc(sizeof(char)*strlen(separators));
        assert( seps != NULL );
        strcpy(seps, separators);
        mdebugf("using custom separators\n");
    }

    int inword = 0;
    int word_length = 0;
    for (int i = 0; i < strlen(input_string); i++) {
        mdebugf("looping i = %d, char = %c\n", i, input_string[i]);
        int found_sep = 0;
        for (int j = 0; j < strlen(seps); j++) {
            if (input_string[i] == seps[j]) {
                mdebugf("found a separator\n");
                found_sep = 1;
            }
        }
        // Also check for the end of the string.
        if (i == strlen(input_string)-1) {
            mdebugf("reached the end of the string\n");
            found_sep = 1;
            // We didn't reach a separator so increment word_length
            // by 1 to account.
            word_length++;
            // And increment i or our string length calculation will be wrong.
            i++;
        }
        if (found_sep) {
            if (inword) {
                mdebugf("reached the end of a word\n");
                inword = 0;
                // malloc a new string in the array and copy it over.
                mdebugf("malloc new string size %d to word %d\n",
                        word_length+1, nwords);
                split_array[nwords] = (char *)malloc(sizeof(char)*word_length);
                assert( split_array[nwords] != NULL );
                assert( i >= word_length );
                mdebugf("copying %d bytes to new string\n", word_length);
                strncpy(split_array[nwords],
                        input_string+i-word_length,
                        word_length);
                nwords++;
            }
        } else if (! inword) {
            inword = 1;
            word_length = 1;
            if (nwords+1 > array_length) {
                array_length *= SPLIT_GROW_FACTOR;
                mdebugf("growing array to size %d\n", sizeof(char *)*array_length);
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
    mdebugf("putting a NULL at word %d\n", nwords);
    split_array[nwords] = NULL;
    mdebugf("returning an array of %d words\n", nwords);
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
