INCLUDES=-I. -I..
LIBS=
CC=gcc
SRC=..
CFLAGS=-g \
	-fprofile-arcs \
	-ftest-coverage \
	-Wall 


all: test_ringbuffer


.PHONY: test
test: all
	./test_ringbuffer


.PHONY: coverage
coverage: test
	gcov -o. $(SRC)/ringbuffer.c


.PHONY: clean
clean:
	-rm ./*.o
	-rm test_ringbuffer
	-rm *.gcov *.gcda *.gcno
	

testing.o: testing.c testing.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $< $(LIBS)
	
ringbuffer.o: $(SRC)/ringbuffer.c $(SRC)/ringbuffer.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $< $(LIBS)

test_ringbuffer: test_ringbuffer.c ringbuffer.o testing.o
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

