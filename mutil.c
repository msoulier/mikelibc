#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "mutil.h"
#include "mdebug.h"
#include "mlog.h"

time_t now;
struct tm now_tm;

int dstnow() {
    time(&now);
    localtime_r(&now, &now_tm);
    return now_tm.tm_isdst;
}

uint64_t fibonacci(int n) {
    mdebugf("fibonacci: n = %d\n", n);
    if (n < 1) {
        return 0;
    } else if (n == 1) {
        return 1;
    } else {
        return fibonacci(n-1) + fibonacci(n-2);
    }
}

uint64_t sfibonacci(int reset) {
    static uint64_t first = 0;
    static uint64_t second = 1;
    static uint64_t count = 0;
    if (reset) {
        first = 0;
        second = 1;
        count = 0;
    }
    count++;
    if (count == 1) {
        return 0;
    } else if (count == 2) {
        return 1;
    } else {
        uint64_t out = first + second;
        first = second;
        second = out;
        return out;
    }
}

int popenRWE(int *rwepipe, const char *exe, const char *const args[])
{
	int in[2];
	int out[2];
	int err[2];
	int pid;
	int rc;

	rc = pipe(in);
	if (rc<0)
		goto error_in;

	rc = pipe(out);
	if (rc<0)
		goto error_out;

	rc = pipe(err);
	if (rc<0)
		goto error_err;

    mdebugf("forking\n");
	pid = fork();
	if (pid > 0) { // parent
        mdebugf("parent: managing file descriptors\n");
        // close reading end of in - reverse for child
		close(in[0]);
        // close writing end of out - reverse for child
		close(out[1]);
        // close writing end of err - reverse for child
		close(err[1]);
		rwepipe[0] = in[1];
		rwepipe[1] = out[0];
		rwepipe[2] = err[0];
        mdebugf("parent: returning pid %d\n", pid);
		return pid;
	} else if (pid == 0) { // child
        mdebugf("child: managing file descriptors\n");
        // don't want to write to in
		close(in[1]);
        // don't want to read from out
		close(out[0]);
        // don't want to read from err
		close(err[0]);
        // close stdin
		close(0);
        // reopen stdin to in
		dup(in[0]);
        // close stdout
		close(1);
        // reopen stdout to out
		dup(out[1]);
        // close stderr
		close(2);
        // reopen stderr to err
		dup(err[1]);

        mdebugf("child: execvp on %s\n", exe);
		execvp(exe, (char**)args);
		exit(1);
	} else
		goto error_fork;

	return pid;

error_fork:
	close(err[0]);
	close(err[1]);
error_err:
	close(out[0]);
	close(out[1]);
error_out:
	close(in[0]);
	close(in[1]);
error_in:
	return -1;
}
