LIBTAIV=0.60
CC=gcc
CFLAGS=-Wall -DLINUX -I./libtai-$(LIBTAIV)
OBJS=mikelib.o mdebug.o mlogger.o
LIBS=

ifeq ($(threads),1)
    CFLAGS += -DMIKELIBC_THREADS
endif

all: libtai.a libmike.a

libtai.a:
	(cd libtai-0.60 && make libtai.a)

libmike.a: $(OBJS)
	ar rc libmike.a $(OBJS)

mikelib.o: mikelib.c mikelib.h
	$(CC) $(CFLAGS) -c mikelib.c

mdebug.o: mdebug.c mdebug.h
	$(CC) $(CFLAGS) -c mdebug.c

mlogger.o: mlogger.c mlogger.h
	$(CC) $(CFLAGS) -c mlogger.c

test: all
	(cd t && make clean && make)
	./t/test_logger

clean:
	rm -f libmike.a $(OBJS)
	(cd t && make clean)
	(cd libtai-$(LIBTAIV) && rm -f *.o libtai.a)
