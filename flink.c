
#include <stdio.h>
#include <assert.h>

#include "mutil.h"
#include "mdebug.h"

#define MAXPATH 4096

int
main(int argc, char *argv[]) {
    char usage[] = "Usage: flink <symlink path>\n"
                   "             [-h]\n";
    char *path = NULL;

    if (argc < 2) {
        fprintf(stderr, usage);
        return 1;
    } else {
        for (int i = 0; i < argc; ++i) {
            if (strncmp(argv[i], "-h", 2) == 0) {
                fprintf(stderr, "Usage: bins [-h]\n");
                return 1;
            } else {
                path = argv[i];
            }
        }
    }

    if (path == NULL) {
        fprintf(stderr, usage);
        return 1;
    }

    char buffer[MAXPATH];
    ssize_t nbytes = followlink(path, buffer, MAXPATH);
    if (nbytes < 0) {
        return 1;
    }
    printf("%s\n", buffer);

    return 0;
}
