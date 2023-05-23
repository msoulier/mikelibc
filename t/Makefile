CC=gcc
CFLAGS=-Wall -I..
LIBS=-L.. -lmike -lcunit
TESTS=
MDEBUG=0

.PHONY: clean test

ifeq ($(MDEBUG),1)
    CFLAGS += -ggdb -DMDEBUG
endif

ifeq ($(ASAN),1)
	CFLAGS += -fsanitize=address
	LIBS += -lasan
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
