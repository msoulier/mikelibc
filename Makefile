LIBTAIV=0.60
#CC=c99
CC=gcc
CFLAGS=-Wall -DLINUX -I./libtai-$(LIBTAIV)
OBJS=mikelib.o mdebug.o mlogger.o mutil.o madt.o mnet.o
LIBS=
MDEBUG=0

.PHONY: clean test

ifeq ($(MDEBUG),1)
    CFLAGS += -ggdb -DMDEBUG -fsanitize=address
endif

ifeq ($(threads),1)
    CFLAGS += -DMIKELIBC_THREADS
endif

all: libtai.a libmike.a

help:
	@echo "MDEBUG is $(MDEBUG)"
	@echo "CFLAGS is $(CFLAGS)"

libtai.a:
	(cd libtai-0.60 && make libtai.a)
	cp libtai-0.60/libtai.a .

libmike.a: $(OBJS)
	ar rc libmike.a $(OBJS)

mikelib.o: mikelib.c mikelib.h
	$(CC) $(CFLAGS) -c mikelib.c

mdebug.o: mdebug.c mdebug.h
	$(CC) $(CFLAGS) -c mdebug.c

mlogger.o: mlogger.c mlogger.h
	$(CC) $(CFLAGS) -c mlogger.c

mutil.o: mutil.c mutil.h
	$(CC) $(CFLAGS) -c mutil.c

madt.o: madt.c madt.h
	$(CC) $(CFLAGS) -c madt.c

mnet.o: mnet.c mnet.h
	$(CC) $(CFLAGS) -c mnet.c

test: all
	(cd t && make clean && make MDEBUG=$(MDEBUG) && make run)

clean:
	rm -f *.a *.o
	(cd t && make clean)
	(cd libtai-$(LIBTAIV) && rm -f *.o libtai.a)
