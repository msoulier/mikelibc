CC=gcc
CFLAGS=-Wall -I..
LIBS=-L.. -lmike
TESTS=
MDEBUG=0
MTHREADS=1

.PHONY: clean perf

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

all: perf

run:
	./perf

perf: perf.o
	$(CC) $(CFLAGS) -o perf perf.o $(LIBS)

clean:
	rm -f perf *.o *.log core
