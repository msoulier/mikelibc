CC=gcc
CFLAGS=-Wall -I..
OBJS=test_logger.o test_mikelib.o
LIBS=-L.. -lmike
TESTS=test_logger test_mikelib

all: $(TESTS) runtests

runtests:
	./test_mikelib
	./test_logger

test_mikelib: test_mikelib.c
	$(CC) $(CFLAGS) -o test_mikelib test_mikelib.c $(LIBS)

test_logger: test_logger.c
	$(CC) $(CFLAGS) -o test_logger test_logger.c $(LIBS)

clean:
	rm -f $(OBJS) $(TESTS)
