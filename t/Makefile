CC=gcc
CFLAGS=-Wall -I.. -DMDEBUG
LIBS=-L.. -lmike -L../libtai-0.60 -ltai -lcunit
TESTS=

all: unittest

run:
	./unittest

unittest: test.o
	$(CC) $(CFLAGS) -o unittest test.o $(LIBS)

clean:
	rm -f unittest *.o
