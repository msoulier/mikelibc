CC=gcc
CFLAGS=-Wall -I..
OBJS=test_logger.o
LIBS=-L.. -lmike

all: test_logger

test_logger: test_logger.c
	$(CC) $(CFLAGS) -o test_logger test_logger.c $(LIBS)

clean:
	rm -f test_logger $(OBJS)
