SRC = ..
INCLUDES=-I./include -I$(SRC)
CC = gcc
LIBS = 
CFLAGS = \
	-g \
	-fprofile-arcs \
	-ftest-coverage \
	-DTESTING=1 \
	-DICACHE_FLASH_ATTR= \
	-Wall 


all: test_ringbuffer


ringbuffer.o: $(SRC)/ringbuffer.c $(SRC)/ringbuffer.h
	$(CC) -c $(CFLAGS) $(INCLUDES) $< -o $@ $(LIBS)

test_ringbuffer: test_ringbuffer.c ringbuffer.o
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

.PHONY: clean 
clean:
	-rm -f ./*.o
	-rm test_ringbuffer
	-rm *.gcov *.gcda *.gcno


.PHONY: test
test: all
	./test_ringbuffer


.PHONY: coverage
coverage: test
	gcov -o. $(SRC)/ringbuffer.c

