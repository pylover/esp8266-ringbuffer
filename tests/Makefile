INCLUDES=-I. -I..
LIBS=
CC=gcc
CFLAGS=-g -Wall


all: test_ringbuffer 


.PHONY: test
test: all
	./test_ringbuffer


.PHONY: clean
clean:
	-rm ./*.o
	-rm test_ringbuffer

testing.o: testing.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $^ $(LIBS)
	
ringbuffer.o: ../ringbuffer.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $^ $(LIBS)

test_ringbuffer: test_ringbuffer.c ringbuffer.o testing.o
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

