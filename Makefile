INCLUDES=-I. 
CC = gcc
LIBS = 
CFLAGS = \
	-g \
	-fprofile-arcs \
	-ftest-coverage \
	-DFUNC_ATTR=\
	-Wall 

TESTS = ./tests
OBJECTS = ringbuffer.o 

all: test_ringbuffer


ringbuffer.o: ringbuffer.c ringbuffer.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $< $(LIBS)


$(TESTS)/testing.o: $(TESTS)/testing.c $(TESTS)/testing.h
	$(CC) $(CFLAGS) $(INCLUDES) -I$(TESTS) -c -o $@ $< $(LIBS)


test_ringbuffer: $(TESTS)/test_ringbuffer.c ringbuffer.o $(TESTS)/testing.o
	$(CC) $(CFLAGS) $(INCLUDES) -I$(TESTS) -o $@ $^ $(LIBS)


.PHONY: clean 
clean:
	-rm -f $(OBJECTS)
	-rm test_ringbuffer
	-rm *.gcov *.gcda *.gcno


.PHONY: test
test: all
	./test_ringbuffer


.PHONY: coverage
coverage: test
	gcov -o. $(SRC)/ringbuffer.c

