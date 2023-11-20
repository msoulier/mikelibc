CC=gcc
# Note: if using -std=c11 then -D_POSIX_C_SOURCE=200112L is required
CFLAGS=-Wall -std=gnu11 -D_GNU_SOURCE
OBJS=mikelib.o mdebug.o mlog.o mutil.o madt.o mnet.o mstring.o base64.o
LIBS=
MDEBUG=0
MTHREADS=1

.PHONY: clean test doc

ifeq ($(MDEBUG),1)
    CFLAGS += -ggdb -DMDEBUG
endif

ifeq ($(ASAN),1)
	CFLAGS += -fsanitize=address
	LIBS += -lasan
endif

ifeq ($(MTHREADS),1)
    CFLAGS += -DMIKELIBC_THREADS
endif

all: libmike.a mb64

help:
	@echo "MDEBUG is $(MDEBUG)"
	@echo "CFLAGS is $(CFLAGS)"

libmike.a: $(OBJS)
	ar rc libmike.a $(OBJS)

base64.o: base64.c base64.h mdebug.h
	$(CC) $(CFLAGS) -c base64.c

mb64.o: mb64.c base64.h
	$(CC) $(CFLAGS) -c mb64.c

mb64: base64.o mb64.o mdebug.o
	$(CC) $(CFLAGS) -o mb64 base64.o mb64.o mdebug.o

mikelib.o: mikelib.c mikelib.h
	$(CC) $(CFLAGS) -c mikelib.c

mdebug.o: mdebug.c mdebug.h
	$(CC) $(CFLAGS) -c mdebug.c

mlog.o: mlog.c mlog.h
	$(CC) $(CFLAGS) -c mlog.c

mutil.o: mutil.c mutil.h
	$(CC) $(CFLAGS) -c mutil.c

mstring.o: mstring.c mstring.h
	$(CC) $(CFLAGS) -c mstring.c

madt.o: madt.c madt.h
	$(CC) $(CFLAGS) -c madt.c

mnet.o: mnet.c mnet.h
	$(CC) $(CFLAGS) -c mnet.c

test: all
	(cd t && make clean && make MDEBUG=$(MDEBUG) && make run)

doc:
	doxygen Doxyfile

clean:
	rm -f *.a *.o core
	rm -rf doc
	(cd t && make clean)
