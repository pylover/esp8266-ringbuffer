#include <stdlib.h>
#include <stdio.h>

#include "ringbuffer.h"
#include "testing.h"


void test_pushone() {
    #define S   5
    char buff[S];
    struct ringbuffer b;
    rb_init(&b, buff, S, RB_OVERFLOW_ERROR);
    eqint(rb_pushone(&b, 'a'), RB_OK);
    eqint(b.writer, 1);
    eqint(b.reader, 0);
    eqchr(buff[0], 'a');
    
    eqint(rb_pushone(&b, 'b'), RB_OK);
    eqint(rb_pushone(&b, 'c'), RB_OK);
    eqint(rb_pushone(&b, 'd'), RB_OK);
    eqint(rb_pushone(&b, 'e'), RB_ERR_FULL);
    eqint(rb_pushone(&b, 'f'), RB_ERR_FULL);
    
    eqnstr(buff, "abcd", 4);
    eqint(b.writer, 4);
    eqint(b.reader, 0);

    /* Change overflow strategy */
    b.overflow = RB_OVERFLOW_IGNORE_NEWER;
    eqint(rb_pushone(&b, 'e'), RB_OK);
    eqnstr(buff, "abcd", 4);
    eqint(b.writer, 4);
    eqint(b.reader, 0);

    b.overflow = RB_OVERFLOW_IGNORE_OLDER;
    eqint(rb_pushone(&b, 'e'), RB_OK);
    eqnstr(buff, "abcde", 5);
    eqint(b.writer, 0);
    eqint(b.reader, 1);

    eqint(rb_pushone(&b, 'f'), RB_OK);
    eqnstr(buff, "fbcde", 5);
    eqint(b.writer, 1);
    eqint(b.reader, 2);
}


int main() {
    test_pushone();
//    char t[9];
//    char *buffer = (char*)malloc(SIZE);
//    RingBuffer rb = {SIZE, 0, 0, buffer};
//    rb_push(&rb, "ABCD", 4);
//    eqint(rb_used(&rb), 4);
//    eqint(rb_available(&rb), 5);
//    eqint(rb.head, 0);
//    eqint(rb.tail, 4);
//    
//    rb_push(&rb, "EFGHI", 5);
//    rb_push(&rb, "JKLMN", 5);
//    eqint(rb_used(&rb), 9);
//    eqint(rb_available(&rb), 0);
//    eqint(rb.head, 5);
//    eqint(rb.tail, 4);
//
//    rb_pop(&rb, t, 9);
//    eqstr(t, "FGHIJKLMN");
//    eqint(rb_used(&rb), 0);
//    eqint(rb_available(&rb), 9);
//    eqint(rb.head, 4);
//    eqint(rb.tail, 4);
//
//    free(buffer);
}

