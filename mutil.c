#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

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

static char base64_chars[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
    };

char *base64_encode(const char *plaintext, size_t input_size) {
    // How much do we need for the crypttext?
    size_t crypt_bytes;
    crypt_bytes = input_size*8/6;
    char *crypttext = (char *)malloc(crypt_bytes);
    assert( crypttext != NULL );

    // We want to convert every 3 bytes to 4 base64 bytes.
    int plain_pos = 0;
    int crypt_pos = 0;
    while (1) {
        uint8_t byte_one = 0;
        uint8_t byte_two = 0;
        uint8_t byte_three = 0;
        uint8_t byte_four = 0;
        int padding = 0;
        // Mask out most significant 6 bits from input byte one.
        byte_one = plaintext[plain_pos] & 0xfc;
        // Mask out the 2 least significant bits from input byte one.
        byte_two = plaintext[plain_pos] & 0x03;
        byte_two = byte_two << 4;
        // And add 4 bits from the next octet if ther is one.
        if (plain_pos+1 < input_size) {
            byte_two = byte_two | (( plaintext[plain_pos+1] & 0xf0 ) >> 2);
            // byte_two is done, need the next 4 bits for byte_three
            byte_three = plaintext[plain_pos+1] & 0x0f;
            byte_three = byte_three << 4;
            if (plain_pos+2 < input_size) {
                // Need 2 more bits to complete byte_three.
                byte_three = byte_three | (( plaintext[plain_pos+2] & 0xc0 ) >> 4 );
                // And byte 4 is the remainder.
                byte_four = plaintext[plain_pos+2] & 0x3f;
                byte_four = byte_four << 2;
            } else {
                padding = 1;
            }
        } else {
            padding = 2;
        }
        crypttext[crypt_pos++] = base64_chars[byte_one];
        crypttext[crypt_pos++] = base64_chars[byte_two];
        if (padding == 2) {
            crypttext[crypt_pos++] = '=';
            crypttext[crypt_pos++] = '=';
        } else if (padding == 1) {
            crypttext[crypt_pos++] = base64_chars[byte_three];
            crypttext[crypt_pos++] = '=';
        } else if (padding == 0) {
            crypttext[crypt_pos++] = base64_chars[byte_three];
            crypttext[crypt_pos++] = base64_chars[byte_four];
        } else {
            assert( 0 );
        }

        plain_pos += 3;
        if (plain_pos >= input_size) {
            break;
        }
    }
    return crypttext;
}

char *base64_decode(const char *crypttext, size_t input_size) {
    return NULL;
}
