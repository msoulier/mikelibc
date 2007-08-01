CC=gcc
CFLAGS=-Wall
OBJS=debug.o logger.o

all: libmike.a

libmike.a: $(OBJS)
	ar rc libmike.a $(OBJS)

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
