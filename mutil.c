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
    mdebugf("base64_encode: plaintext %s, input_size %d", plaintext, input_size);
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
        byte_one >>= 2;
        // Mask out the 2 least significant bits from input byte one and
        // put them onto output byte_two
        byte_two = plaintext[plain_pos] & 0x03;
        // Make room for the next 4 bits
        byte_two <<= 4;
        // And add 4 bits from the next octet if there is one.
        if (plain_pos+1 < input_size) {
            // Grab most significant 4 bits from next byte and shift right
            // 4 to or them into output byte_two
            byte_two |= ( plaintext[plain_pos+1] & 0xf0 ) >> 4;
            // byte_two is done, need the next 4 bits for byte_three
            byte_three = plaintext[plain_pos+1] & 0x0f;
            // Make room for the next 2 bits.
            byte_three <<= 2;
            if (plain_pos+2 < input_size) {
                // Need 2 more bits to complete byte_three, and shift right 4.
                byte_three |= ( plaintext[plain_pos+2] & 0xc0 ) >> 6;
                // And byte 4 is the remainder.
                byte_four = plaintext[plain_pos+2] & 0x3f;
            } else {
                padding = 1;
            }
        } else {
            padding = 2;
        }
        crypttext[crypt_pos++] = base64_chars[byte_one];
        mdebugf("crypttext one is %c", base64_chars[byte_one]);
        crypttext[crypt_pos++] = base64_chars[byte_two];
        mdebugf("crypttext two is %c", base64_chars[byte_two]);
        if (padding == 2) {
            crypttext[crypt_pos++] = '=';
            crypttext[crypt_pos++] = '=';
            mdebugf("padding last two bytes with =");
        } else if (padding == 1) {
            crypttext[crypt_pos++] = base64_chars[byte_three];
            mdebugf("crypttext three is %c", base64_chars[byte_three]);
            crypttext[crypt_pos++] = '=';
            mdebugf("padding last byte with =");
        } else if (padding == 0) {
            crypttext[crypt_pos++] = base64_chars[byte_three];
            mdebugf("crypttext three is %c", base64_chars[byte_three]);
            crypttext[crypt_pos++] = base64_chars[byte_four];
            mdebugf("crypttext four is %c", base64_chars[byte_four]);
        } else {
            assert( 0 );
        }
        assert( byte_one <= 64 );
        assert( byte_two <= 64 );
        assert( byte_three <= 64 );
        assert( byte_four <= 64 );

        plain_pos += 3;
        if (plain_pos >= input_size) {
            break;
        }
    }
    crypttext[crypt_pos] = '\0';
    return crypttext;
}

char *base64_decode(const char *crypttext, size_t input_size) {
    size_t plaintext_bytes = input_size*6/8;
    mdebugf("base64_decode: crypttext %s, size %d", crypttext, input_size);
    char *plaintext = (char *)malloc(plaintext_bytes);
    assert( plaintext != NULL );

    for (int i = 0; i < input_size; i++) {
        char c = crypttext[i];
        printf("dec %d, oct %o, hex %x, %c\n", c, c, c, c);
    }

    int plaintext_pos = 0;
    int crypttext_pos = 0;
    while (1) {
        // Loop four bytes at a time, turning them into three bytes by
        // moving from 6-bits per 8-bits.
        uint8_t byte_one = 0;
        uint8_t byte_two = 0;
        uint8_t byte_three = 0;

        if (crypttext[crypttext_pos] == '=') {
            break;
        }
        byte_one = crypttext[crypttext_pos] & 0x3f;

        // And the next two bits from the next byte
        if (crypttext[crypttext_pos+1] == '=') {
            break;
        }
        byte_one |= (crypttext[crypttext_pos+1] & 0x03) << 6;
        mdebugf("byte_one is %d %c", byte_one, byte_one);
        // Next 4 bits to byte_two
        byte_two = (crypttext[crypttext_pos+1] & 0x3c) >> 2;

        if (crypttext[crypttext_pos+2] == '=') {
            break;
        }
        // Next 4 bits to complete it
        byte_two |= (crypttext[crypttext_pos+2] & 0x0f) << 4;
        // byte_three is now 2 bits from this bytes plus 6 bits
        // from the next one.
        byte_three = (crypttext[crypttext_pos+2] & 0x30) >> 4;

        if (crypttext[crypttext_pos+3] == '=') {
            break;
        }
        byte_three |= (crypttext[crypttext_pos+3] & 0x3f) << 2;
        crypttext_pos += 4;
        if (crypttext_pos >= input_size) {
            break;
        }

        plaintext[plaintext_pos++] = byte_one;
        plaintext[plaintext_pos++] = byte_two;
        plaintext[plaintext_pos++] = byte_three;
    }
    plaintext[plaintext_pos] = '\0';
    return plaintext;
}
