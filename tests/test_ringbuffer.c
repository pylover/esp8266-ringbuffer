#include <stdlib.h>
#include <stdio.h>

#include "ringbuffer.h"
#include "testing.h"


void test_pushone() {
    #define S   5
    char buff[S];
    struct ringbuffer b;
    rb_init(&b, buff, S, RB_OVERFLOW_ERROR);
    eqint(S, b.size);
    eqint(rb_available(&b), S-1);

    eqint(rb_pushone(&b, 'a'), RB_OK);
    eqint(b.writer, 1);
    eqint(b.reader, 0);
    eqchr(buff[0], 'a');
    eqint(rb_available(&b), 3);
    
    eqint(rb_pushone(&b, 'b'), RB_OK);
    eqint(rb_pushone(&b, 'c'), RB_OK);
    eqint(rb_pushone(&b, 'd'), RB_OK);
    eqint(rb_available(&b), 0);

    eqint(rb_pushone(&b, 'e'), RB_ERR_FULL);
    eqint(rb_pushone(&b, 'f'), RB_ERR_FULL);
    eqint(rb_available(&b), 0);
    eqnstr(buff, "abcd", 4);
    eqint(b.writer, 4);
    eqint(b.reader, 0);

    /* Change overflow strategy */
    b.overflow = RB_OVERFLOW_IGNORE_NEWER;
    eqint(rb_pushone(&b, 'e'), RB_OK);
    eqnstr(buff, "abcd", 4);
    eqint(b.writer, 4);
    eqint(b.reader, 0);
    eqint(rb_available(&b), 0);

    b.overflow = RB_OVERFLOW_IGNORE_OLDER;
    eqint(rb_pushone(&b, 'e'), RB_OK);
    eqnstr(buff, "abcde", 5);
    eqint(b.writer, 0);
    eqint(b.reader, 1);
    eqint(rb_available(&b), 0);

    eqint(rb_pushone(&b, 'f'), RB_OK);
    eqnstr(buff, "fbcde", 5);
    eqint(b.writer, 1);
    eqint(b.reader, 2);
    eqint(rb_available(&b), 0);
}


void test_push() {
    #define S   5
    char buff[S];
    struct ringbuffer b;
    rb_init(&b, buff, S, RB_OVERFLOW_ERROR);

    eqint(rb_push(&b, "abcde", 5), RB_ERR_INSUFFICIENT);
    eqint(rb_push(&b, "abcd", 4), RB_OK);
    eqnstr(buff, "abcd", 4);
    
    rb_reset(&b);

    /* Change overflow strategy */
    b.overflow = RB_OVERFLOW_IGNORE_NEWER;
    rb_reset(&b);
    eqint(rb_push(&b, "abcdefg", 7), RB_OK);
    eqnstr(buff, "abcde", 5);

    b.overflow = RB_OVERFLOW_IGNORE_OLDER;
    rb_reset(&b);
    eqint(rb_push(&b, "abcdefg", 7), RB_OK);
    eqnstr(buff, "fgcde", 5);
}


int main() {
    test_pushone();
    test_push();
}

