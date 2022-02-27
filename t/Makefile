CC=gcc
CFLAGS=-Wall -I..
LIBS=-L.. -lmike -L../libtai-0.60 -ltai -lcunit
TESTS=
MDEBUG=0

.PHONY: clean test

ifeq ($(MDEBUG),1)
    CFLAGS += -ggdb -DMDEBUG
endif

ifeq ($(threads),1)
    CFLAGS += -DMIKELIBC_THREADS
endif

all: test

run:
	./test

test: test.o
	$(CC) $(CFLAGS) -o test test.o $(LIBS)

clean:
	rm -f test *.o *.log
