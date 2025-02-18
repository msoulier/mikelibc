#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <curl/curl.h>
#include <sys/stat.h>

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

char *base64_encode_openssl(const unsigned char *data, size_t input_size) {
	int b64string_mem = 4*((input_size+2)/3);
    // +1 for terminating null that EVP_EncodeBlock adds
    char *b64string = (char *)malloc(b64string_mem+1);
    int bytes = EVP_EncodeBlock((unsigned char *)b64string, data, input_size);
    if (bytes < 0) {
        merrorf("base64_encode: EVP_EncodeBlock returned an error");
        free(b64string);
        return NULL;
    }
    if (b64string_mem != bytes) {
        mwarningf("base64_encode: expected %d bytes but got %d",
            b64string_mem, bytes);
    }
    return b64string;
}

unsigned char *base64_decode_openssl(const char *b64string, size_t *output_size) {
    size_t input_size = strlen(b64string);
    int data_mem = 3*input_size/4;
    unsigned char *data = (unsigned char *)malloc(data_mem);
    mdbgf("base64_decode: calling EVP_DecodeBlock with size %d\n", input_size);
    int bytes = EVP_DecodeBlock(data, (unsigned char *)b64string, input_size);
    if (bytes < 0) {
        merrorf("base64_decode: EVP_DecodeBlock returned an error");
        free(data);
        return NULL;
    }
    if (bytes != data_mem) {
        mwarningf("base64_decode: expected %d bytes but got %d",
            data_mem, bytes);
    }
    // Reduce the bytes returned by the number of padding bytes in the
    // original encoded string.
    if ((input_size > 0) && (b64string[input_size-1] == '=')) {
        mdbgf("one = found at %d\n", input_size-1);
        bytes--;
    }
    if ((input_size > 1) && (b64string[input_size-2] == '=')) {
        mdbgf("another = found at %d\n", input_size-2);
        bytes--;
    }
    *output_size = bytes;
    return data;
}

unsigned char *encrypt_ssl(const unsigned char *key,
                           const unsigned char *iv,
                           const EVP_CIPHER *cipher_type,
                           const unsigned char *plaintext,
                           int input_size)
{
    int outlen = 0;

    // FIXME: how much do we need? Initially aim for input*2 to be safe.
    char *crypttext = (char *)malloc(sizeof(char)*input_size*2);
    assert( crypttext != NULL );

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);

    if (cipher_type == NULL) {
        EVP_EncryptInit_ex(ctx, EVP_aes_256_cfb8(), NULL, key, iv);
    } else {
        EVP_EncryptInit_ex(ctx, cipher_type, NULL, key, iv);
    }

    unsigned char *crypttext_p = (unsigned char *)crypttext;
    for (int i = 0; i < input_size; i++) {
        if (!EVP_EncryptUpdate(ctx, crypttext_p, &outlen, &plaintext[i], 1)) {
            merrorf("encrypt_aes: EVP_EncryptUpdate returned an error");
            free(crypttext);
            return NULL;
        }
        crypttext_p += outlen;
    }

    if (!EVP_EncryptFinal(ctx, crypttext_p, &outlen)) {
        merrorf("encrypt_aes: EVP_EncryptFinal returned an error");
        free(crypttext);
        return NULL;
    }
    crypttext_p += outlen;
    *crypttext_p = '\0';

    EVP_CIPHER_CTX_free(ctx);

    return (unsigned char *)crypttext;
}

unsigned char *decrypt_ssl(const unsigned char *key,
                           const unsigned char *iv,
                           const EVP_CIPHER *cipher_type,
                           const unsigned char *ciphertext,
                           int input_size)
{
    int outlen = 0;

    // FIXME: how much do we need? Initially aim for input*2 to be safe.
    char *plaintext = (char *)malloc(sizeof(char)*input_size*2);
    assert( plaintext != NULL );

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);

    if (cipher_type == NULL) {
        EVP_DecryptInit_ex(ctx, EVP_aes_256_cfb8(), NULL, key, iv);
    } else {
        EVP_DecryptInit_ex(ctx, cipher_type, NULL, key, iv);
    }

    unsigned char *plaintext_p = (unsigned char *)plaintext;
    for (int i = 0; i < input_size; i++) {
        if (!EVP_DecryptUpdate(ctx, plaintext_p, &outlen, &ciphertext[i], 1)) {
            merrorf("decrypt_aes: EVP_DecryptUpdate returned an error");
            free(plaintext);
            return NULL;
        }
        plaintext_p += outlen;
    }

    if (!EVP_DecryptFinal(ctx, plaintext_p, &outlen)) {
        merrorf("decrypt_aes: EVP_DecryptFinal returned an error");
        free(plaintext);
        return NULL;
    }
    plaintext_p += outlen;
    *plaintext_p = '\0';

    EVP_CIPHER_CTX_free(ctx);

    return (unsigned char *)plaintext;
}

unsigned char *digest_sha1(const unsigned char *in,
                           size_t in_length,
                           unsigned int *digest_len)
{
    EVP_MD_CTX *mdctx;
    unsigned char *digest = NULL;

    if ((mdctx = EVP_MD_CTX_new()) == NULL) {
		goto CLEANUP;
    }

	if (1 != EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL)) {
		goto CLEANUP;
    }

	if (1 != EVP_DigestUpdate(mdctx, in, in_length)) {
		goto CLEANUP;
    }

	if ((digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha1()))) == NULL) {
		goto CLEANUP;
    }

	if (1 != EVP_DigestFinal_ex(mdctx, digest, digest_len)) {
        OPENSSL_free(digest);
		goto CLEANUP;
    }

CLEANUP:
    EVP_MD_CTX_free(mdctx);
    return digest;
}

char *tohex(const unsigned char *in,
            size_t in_length)
{
    char hexbuf[3];
    char *hexstring = NULL;
    // Should be x2 the input length.
    hexstring = (char *)malloc((sizeof(char)*in_length*2)+1);
    if (hexstring == NULL) {
        return NULL;
    }
    int index = 0;
    for (size_t i = 0; i < in_length; i++) {
        int c = in[i];
        sprintf(hexbuf, "%02x", c);
        hexstring[index++] = hexbuf[0];
        hexstring[index++] = hexbuf[1];
    }
    return hexstring;
}

char *uriencode(const char *in, int *out_length) {
    char *out_string = NULL;
    size_t max_output = CURL_MAX_READ_SIZE*3;
    CURL *curl = curl_easy_init();
    char *c_encoded = curl_easy_escape(curl, in, 0);
    *out_length = strnlen(c_encoded, max_output);
    out_string = malloc(*out_length*sizeof(char));
    if (out_string == NULL) {
        goto CLEANUP;
    }
    strncpy(out_string, c_encoded, *out_length);
CLEANUP:
    curl_free(c_encoded);
    curl_easy_cleanup(curl);
    return out_string;
}

 char *uridecode(const char *in, int *out_length) {
    char *out_string = NULL;
    size_t max_output = CURL_MAX_READ_SIZE;
    CURL *curl = curl_easy_init();
    char *c_decoded = curl_easy_unescape(curl, in, 0, out_length);
    *out_length = strnlen(c_decoded, max_output);
    out_string = malloc(*out_length*sizeof(char));
    if (out_string == NULL) {
        goto CLEANUP;
    }
    strncpy(out_string, c_decoded, *out_length);
CLEANUP:
    curl_free(c_decoded);
    curl_easy_cleanup(curl);
    return out_string;
 }

ssize_t followlink(const char *pathname,
                   char *buf,
                   size_t bufsiz)
{
    ssize_t nbytes = readlink(pathname, buf, bufsiz);
    mdbgf("followlink: readlink wrote %d bytes\n", nbytes);
    if (nbytes < 0) {
        return nbytes;
    } else {
        // readlink does not null terminate
        buf[nbytes] = '\0';
        mdbgf("followlink: buf is %s\n", buf);
    }
    // Are we still pointing at a symlink?
    struct stat statbuf;
    if (lstat(buf, &statbuf) < 0) {
        perror("stat");
        return -1;
    }
    switch (statbuf.st_mode & S_IFMT) {
        case S_IFLNK:
            mdbgf("followlink: %s is a symlink, continuing\n", buf);
            char pathname[MAXPATH];
            strncpy(pathname, buf, nbytes);
            return followlink(pathname, buf, bufsiz);
            break;
        default:
            mdbgf("followlink: %s is not a symlink, returning\n", buf);
            return nbytes;
    }
}
