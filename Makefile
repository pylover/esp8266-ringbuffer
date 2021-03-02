INCLUDES=-I. 
CC = gcc
LIBS = 
CFLAGS = \
	-Wall 

OBJECTS = ringbuffer.o 

ringbuffer.o: ringbuffer.c ringbuffer.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $^ $(LIBS)

.PHONY: clean 
clean:
	rm -f $(OBJECTS)

#.PHONY: test
#test:
#	./upper -v


