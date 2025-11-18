#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "mstr.h"
#include "mdebug.h"
#include "mlog.h"

#define MAXPATH 4096

void
find_bins(const char * const dirpath, int full) {
    assert( dirpath != NULL );
    mdebugf("===> walking dirpath %s\n", dirpath);
    struct stat statbuf;
    DIR *current_dir = opendir(dirpath);
    if (current_dir == NULL) {
        mdebugf("opendir: %s\n", strerror(errno));
        return;
    }
    for (;;) {
        struct dirent *entry = readdir(current_dir);
        if (entry == NULL) {
            break;
        }
        if ((entry->d_type == DT_REG) || (entry->d_type == DT_LNK)) {
            if (entry->d_type == DT_LNK) {
                mdebugf("symbolic link: %s\n", entry->d_name);
            } else {
                mdebugf("regular file: %s\n", entry->d_name);
            }
            char path[MAXPATH];
            sprintf(path, "%s/%s", dirpath, entry->d_name);
            mdebugf("stat of %s\n", path);
            if (stat(path, &statbuf) < 0) {
                perror("stat");
                continue;
            }
            mdebugf("mode: %jo\n", statbuf.st_mode);
            // Do we have ownership, group or other execute permission?
            if (statbuf.st_mode & S_IXUSR|S_IXGRP|S_IXOTH) {
                mdebugf("===> executable: %s\n", path);
                if (full) {
                    printf("%s\n", path);
                } else {
                    printf("%s\n", entry->d_name);
                }
            }
        }
    }
}

int
main(int argc, char *argv[]) {
    int full = 0;

    if (argc > 1) {
        for (int i = 0; i < argc; ++i) {
            if (strncmp(argv[i], "-h", 2) == 0) {
                fprintf(stderr, "Usage: bins [-h]\n");
                return 1;
            } else if (strncmp(argv[i], "-f", 2) == 0) {
                full = 1;
            }
        }
    }
#ifdef MDEBUG
	// Give the libraries something to log to.
	mlog_handle_t handle = get_mlogger(MLOG_STDOUT, MLOG_DEBUG, UTC);
#endif

    char path[MAXPATH];
    strncpy(path, getenv("PATH"), MAXPATH);
    mdebugf("PATH is %s\n", path);
    char **pieces = msplit(path, ":");

    int i = 0;
    for (;;) {
        char *piece = pieces[i];
        if (piece == NULL) {
            break;
        }
        mdebugf("%s\n", piece);
        find_bins(piece, full);
        ++i;
    }
    free_msplit(pieces);
}
