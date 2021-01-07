CC=gcc
CFLAGS=-Wall -I..
OBJS=test_logger.o test_mikelib.o test_adt.o
LIBS=-L.. -lmike -L../libtai-0.60 -ltai
TESTS=test_logger test_mikelib test_util test_adt

all: $(TESTS) runtests

runtests:
	-./test_mikelib
	-./test_logger
	-./test_util
	-./test_adt

test_util: test_util.c ../mutil.h ../libmike.a
	$(CC) $(CFLAGS) -o test_util test_util.c $(LIBS)

test_mikelib: test_mikelib.c ../libmike.a
	$(CC) $(CFLAGS) -o test_mikelib test_mikelib.c $(LIBS)

test_logger: test_logger.c ../mlogger.h ../libmike.a
	$(CC) $(CFLAGS) -o test_logger test_logger.c $(LIBS)

test_adt: test_adt.c ../madt.h ../libmike.a
	$(CC) $(CFLAGS) -o test_adt test_adt.c $(LIBS)

trylog: trylog.c ../libmike.a
	$(CC) $(CFLAGS) -o trylog trylog.c $(LIBS)

clean:
	rm -f $(OBJS) $(TESTS) trylog mylogfile
