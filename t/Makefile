CC=gcc
CFLAGS=-Wall -std=gnu11 -D_GNU_SOURCE -I..
LIBS=-L.. -lmike -lcunit -lssl -lcrypto
TESTS=
MDEBUG=0
MTHREADS=1

.PHONY: clean test

ifeq ($(MDEBUG),1)
    CFLAGS += -ggdb -DMDEBUG
endif

ifeq ($(ASAN),1)
	CFLAGS += -fsanitize=address
	LIBS += -lasan
endif

ifeq ($(MTHREADS),1)
    CFLAGS += -DMIKELIBC_THREADS
	LIBS += -lpthread
endif

all: test

run:
	./test

test: test.o
	$(CC) $(CFLAGS) -o test test.o $(LIBS)

clean:
	rm -f test *.o *.log core
