CC = gcc
LIBS = 
CFLAGS = -Wall -I.
OBJECTS = ringbuffer.o 

ringbuffer.o: ringbuffer.c ringbuffer.h

.PHONY: clean 
clean:
	rm -f $(OBJECTS)

#.PHONY: test
#test:
#	./upper -v


