LIBTAIV=0.60
CC=gcc
CFLAGS=-Wall -DLINUX -I./libtai-$(LIBTAIV)
OBJS=mikelib.o debug.o logger.o

all: libtai.a libmike.a

libtai.a:
	(cd libtai-0.60 && make libtai.a)

libmike.a: $(OBJS)
	ar rc libmike.a $(OBJS)

mikelib.o: mikelib.c mikelib.h
	$(CC) $(CFLAGS) -c mikelib.c

debug.o: debug.c debug.h
	$(CC) $(CFLAGS) -c debug.c

logger.o: logger.c logger.h
	$(CC) $(CFLAGS) -c logger.c

test: all
	(cd t && make clean && make)
	./t/test_logger

clean:
	rm -f libmike.a $(OBJS)
	(cd t && make clean)
	(cd libtai-$(LIBTAIV) && rm -f *.o libtai.a)
