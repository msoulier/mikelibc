LIBTAIV=0.60
CC=gcc
CFLAGS=-Wall -DLINUX -I./libtai-$(LIBTAIV)
OBJS=mikelib.o mdebug.o mlogger.o mutil.o madt.o
LIBS=
DEBUG=0

ifeq ($(DEBUG),1)
	CFLAGS += -ggdb -fsanitize=address
endif

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

mutil.o: mutil.c mutil.h
	$(CC) $(CFLAGS) -c mutil.c

madt.o: madt.c madt.h
	$(CC) $(CFLAGS) -c madt.c

test: all
	(cd t && make clean && make)

clean:
	rm -f libmike.a $(OBJS)
	(cd t && make clean)
	(cd libtai-$(LIBTAIV) && rm -f *.o libtai.a)
