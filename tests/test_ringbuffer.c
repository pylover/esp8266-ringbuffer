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

    eqint(rb_pushone(&b, 'e'), RB_ERR_INSUFFICIENT);
    eqint(rb_pushone(&b, 'f'), RB_ERR_INSUFFICIENT);
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


void test_write_read() {
    #define S   5
    char tmp[256];
    int tmplen = 0;
    char buff[S];
    struct ringbuffer b;
    rb_init(&b, buff, S, RB_OVERFLOW_ERROR);

    eqint(rb_write(&b, "abcde", 5), RB_ERR_INSUFFICIENT);
    eqint(rb_write(&b, "abcd", 4), RB_OK);
    eqnstr(buff, "abcd", 4);
    
    rb_reset(&b);

    /* Change overflow strategy */
    b.overflow = RB_OVERFLOW_IGNORE_NEWER;
    rb_reset(&b);
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqnstr(buff, "abcd", 4);

    b.overflow = RB_OVERFLOW_IGNORE_OLDER;
    rb_reset(&b);
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqnstr(buff, "fgcde", 5);
   
    /* Write & Read */
    rb_reset(&b);
    eqint(rb_write(&b, "abcd", 4), RB_OK);
    eqint(b.writer, 4);
    tmplen += rb_read(&b, tmp + tmplen, 10);
    eqint(tmplen, 4); 
    eqnstr(tmp, "abcd", 4);
    eqint(b.writer, 4);
    eqint(b.reader, 4);
    eqint(rb_available(&b), 4);

    eqint(rb_write(&b, "ef", 2), RB_OK);
    tmplen += rb_read(&b, tmp + tmplen, 10);
    eqint(tmplen, 6); 

    eqint(rb_write(&b, "ghij", 4), RB_OK);
    tmplen += rb_read(&b, tmp + tmplen, 2);
    tmplen += rb_read(&b, tmp + tmplen, 2);
    eqint(tmplen, 10); 
    eqnstr(tmp, "abcdefghij", 10);
}


void test_dry_read() {
    #define S   8
    char tmp[256];
    int tmplen = 0;
    char buff[S];
    struct ringbuffer b;
    rb_init(&b, buff, S, RB_OVERFLOW_ERROR);

    /* Dry Read */
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    tmplen += rb_dryread(&b, tmp + tmplen, 2);
    eqint(tmplen, 2); 
    eqnstr(buff, "abcdefg", 7);
    eqnstr(tmp, "ab", 2);
    eqint(b.writer, 7);
    eqint(b.reader, 0);

    rb_reader_skip(&b, 2);
    tmplen += rb_dryread(&b, tmp + tmplen, 10);
    eqint(tmplen, 7); 
    eqnstr(buff, "abcdefg", 7);
    eqnstr(tmp, "abcdefg", 7);
    eqint(b.writer, 7);
    eqint(b.reader, 2);
    
    /* Skip */
    rb_reader_skip(&b, 4);
    eqint(b.reader, 6);
    eqint(rb_available(&b), 6);
}


int main() {
    test_pushone();
    test_write_read();
    test_dry_read();
}

