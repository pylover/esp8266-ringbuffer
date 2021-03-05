#include <stdlib.h>
#include <stdio.h>

#include "ringbuffer.h"
#include "testing.h"

#define S   8

void test_pushone() {
    char buff[S];
    struct ringbuffer b;
    rb_init(&b, buff, S, RB_OVERFLOW_ERROR);
    eqint(S, b.size);
    eqint(RB_AVAILABLE(&b), S-1);
    
    eqint(rb_pushone(&b, 'a'), RB_OK);
    eqint(b.writer, 1);
    eqint(b.reader, 0);
    eqchr(buff[0], 'a');
    eqint(RB_AVAILABLE(&b), 6);
    
    eqint(rb_pushone(&b, 'b'), RB_OK);
    eqint(rb_pushone(&b, 'c'), RB_OK);
    eqint(rb_pushone(&b, 'd'), RB_OK);
    eqint(RB_AVAILABLE(&b), 3);

    eqint(rb_pushone(&b, 'e'), RB_OK);
    eqint(rb_pushone(&b, 'f'), RB_OK);
    eqint(rb_pushone(&b, 'g'), RB_OK);
    eqint(RB_AVAILABLE(&b), 0);

    eqint(rb_pushone(&b, 'h'), RB_ERR_INSUFFICIENT);
    eqint(RB_AVAILABLE(&b), 0);
    eqnstr(buff, "abcdefg", 7);
    eqint(b.writer, 7);
    eqint(b.reader, 0);

    /* Change overflow strategy */
    b.overflow = RB_OVERFLOW_IGNORE_NEWER;
    eqint(rb_pushone(&b, 'h'), RB_OK);
    eqnstr(buff, "abcdefg", 7);
    eqint(b.writer, 7);
    eqint(b.reader, 0);
    eqint(RB_AVAILABLE(&b), 0);

    b.overflow = RB_OVERFLOW_IGNORE_OLDER;
    eqint(rb_pushone(&b, 'h'), RB_OK);
    eqnstr(buff, "abcdefgh", 8);
    eqint(b.writer, 0);
    eqint(b.reader, 1);
    eqint(RB_AVAILABLE(&b), 0);

    eqint(rb_pushone(&b, 'i'), RB_OK);
    eqnstr(buff, "ibcdefgh", 8);
    eqint(b.writer, 1);
    eqint(b.reader, 2);
    eqint(RB_AVAILABLE(&b), 0);
}


void test_write_read() {
    char tmp[256];
    int tmplen = 0;
    char buff[S];
    struct ringbuffer b;
    rb_init(&b, buff, S, RB_OVERFLOW_ERROR);

    eqint(rb_write(&b, "abcdefgh", 8), RB_ERR_INSUFFICIENT);
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqnstr(buff, "abcdefg", 7);
   
    /* Change overflow strategy */
    b.overflow = RB_OVERFLOW_IGNORE_NEWER;
    RB_RESET(&b);
    eqint(rb_write(&b, "abcdefghijk", 11), RB_OK);
    eqnstr(buff, "abcdefg", 7);

    b.overflow = RB_OVERFLOW_IGNORE_OLDER;
    RB_RESET(&b);
    eqint(rb_write(&b, "abcdefghijk", 11), RB_OK);
    eqnstr(buff, "ijkdefgh", 8);
    eqint(b.writer, 3);
    eqint(b.reader, 4);
    eqint(RB_AVAILABLE(&b), 0);
   
    /* Write & Read */
    RB_RESET(&b);
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqint(b.writer, 7);
    tmplen += rb_read(&b, tmp + tmplen, 10);
    eqint(tmplen, 7); 
    eqnstr(tmp, "abcdefg", 7);
    eqint(b.writer, 7);
    eqint(b.reader, 7);
    eqint(RB_AVAILABLE(&b), 7);

    eqint(rb_write(&b, "hi", 2), RB_OK);
    tmplen += rb_read(&b, tmp + tmplen, 10);
    eqint(tmplen, 9); 

    eqint(rb_write(&b, "jklm", 4), RB_OK);
    tmplen += rb_read(&b, tmp + tmplen, 2);
    tmplen += rb_read(&b, tmp + tmplen, 2);
    eqint(tmplen, 13); 
    eqnstr(tmp, "abcdefghijklm", 13);
    
    /* Read when no data available */
    eqint(rb_read(&b, tmp, 2), 0);
}


void test_dry_read() {
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

    RB_READER_SKIP(&b, 2);
    tmplen += rb_dryread(&b, tmp + tmplen, 10);
    eqint(tmplen, 7); 
    eqnstr(buff, "abcdefg", 7);
    eqnstr(tmp, "abcdefg", 7);
    eqint(b.writer, 7);
    eqint(b.reader, 2);
    
    /* Skip */
    RB_READER_SKIP(&b, 4);
    eqint(b.reader, 6);
    eqint(RB_AVAILABLE(&b), 6);
}


void test_read_until() {
    char tmp[256];
    rb_size_t tmplen = 0;
    char buff[S];
    struct ringbuffer b;
    rb_init(&b, buff, S, RB_OVERFLOW_ERROR);

    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqint(rb_read_until(&b, tmp, 7, "de", 2, &tmplen), RB_OK);
    eqint(tmplen, 5); 
    eqnstr(tmp, "abc", 3);
    eqint(b.writer, 7);
    eqint(b.reader, 5);
    
    tmplen = 0;
    RB_RESET(&b);
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqint(rb_read_until(&b, tmp, 7, "ed", 2, &tmplen), RB_ERR_NOTFOUND);
    eqint(tmplen, 0); 
    eqnstr(tmp, "abcdefg", 7);
    eqint(b.writer, 7);
    eqint(b.reader, 0);

    tmplen = 0;
    RB_RESET(&b);
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqint(rb_read_until(&b, tmp, 8, "ed", 2, &tmplen), RB_ERR_NOTFOUND);
    eqint(tmplen, 0); 
    eqnstr(tmp, "abcdefg", 7);
    eqint(b.writer, 7);
    eqint(b.reader, 0);
}


int main() {
    test_pushone();
    test_write_read();
    test_dry_read();
    test_read_until();
}

