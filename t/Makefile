CC=gcc
CFLAGS=-Wall -I.. -DMDEBUG
LIBS=-L.. -lmike -L../libtai-0.60 -ltai -lcunit
TESTS=

.PHONY: clean test

all: test

run:
	./test 2>stderr.log

test: test.o
	$(CC) $(CFLAGS) -o test test.o $(LIBS)

clean:
	rm -f test *.o *.log
