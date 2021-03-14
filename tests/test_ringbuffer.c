#include <stdlib.h>
#include <stdio.h>

#include "ringbuffer.h"
#include "testing.h"

#define S   8


void test_write_read() {
    char tmp[256];
    int tmplen = 0;
    char buff[S];
    struct ringbuffer b;
    rb_init(&b, buff, S);

    eqint(rb_write(&b, "abcdefgh", 8), RB_ERR_INSUFFICIENT);
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqnstr(buff, "abcdefg", 7);
    eqint(b.writer, 7);
    eqint(b.reader, 0);
    eqint(b.writecounter, 7);
   
    /* Write & Read */
    RB_RESET(&b);
    eqint(b.writecounter, 0);
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqint(b.writer, 7);
    tmplen += rb_read(&b, tmp + tmplen, 10);
    eqint(tmplen, 7); 
    eqnstr(tmp, "abcdefg", 7);
    eqint(b.writer, 7);
    eqint(b.reader, 7);
    eqint(RB_AVAILABLE(&b), 7);
    eqint(b.writecounter, 7);

    eqint(rb_write(&b, "hi", 2), RB_OK);
    tmplen += rb_read(&b, tmp + tmplen, 10);
    eqint(tmplen, 9); 
    eqint(b.writecounter, 9);

    eqint(rb_write(&b, "jklm", 4), RB_OK);
    tmplen += rb_read(&b, tmp + tmplen, 2);
    tmplen += rb_read(&b, tmp + tmplen, 2);
    eqint(tmplen, 13); 
    eqnstr(tmp, "abcdefghijklm", 13);
    eqint(b.writecounter, 13);
    
    /* Read when no data available */
    eqint(rb_read(&b, tmp, 2), 0);
    eqint(b.writecounter, 13);
}


void test_dryread() {
    char tmp[256];
    int tmplen = 0;
    char buff[S];
    struct ringbuffer b;
    rb_init(&b, buff, S);

    /* Dry Read */
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    tmplen += rb_dryread(&b, tmp + tmplen, 2);
    eqint(tmplen, 2); 
    eqnstr(buff, "abcdefg", 7);
    eqnstr(tmp, "ab", 2);
    eqint(b.writer, 7);
    eqint(b.reader, 0);
    eqint(b.writecounter, 7);

    RB_READER_SKIP(&b, 2);
    tmplen += rb_dryread(&b, tmp + tmplen, 10);
    eqint(tmplen, 7); 
    eqnstr(buff, "abcdefg", 7);
    eqnstr(tmp, "abcdefg", 7);
    eqint(b.writer, 7);
    eqint(b.reader, 2);
    eqint(b.writecounter, 7);
    
    /* Skip */
    RB_READER_SKIP(&b, 4);
    eqint(b.reader, 6);
    eqint(RB_AVAILABLE(&b), 6);
    eqint(b.writecounter, 7);
}


void test_read_until() {
    char tmp[256];
    size16_t tmplen = 0;
    char buff[S];
    struct ringbuffer b;
    rb_init(&b, buff, S);

    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqint(rb_read_until(&b, tmp, 7, "de", 2, &tmplen), RB_OK);
    eqint(tmplen, 5); 
    eqnstr(tmp, "abc", 3);
    eqint(b.writer, 7);
    eqint(b.reader, 5);
    eqint(b.writecounter, 7);
    
    tmplen = 0;
    RB_RESET(&b);
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqint(rb_read_until(&b, tmp, 7, "ed", 2, &tmplen), RB_ERR_NOTFOUND);
    eqint(tmplen, 7); 
    eqnstr(tmp, "abcdefg", 7);
    eqint(b.writer, 7);
    eqint(b.reader, 0);
    eqint(b.writecounter, 7);

    tmplen = 0;
    RB_RESET(&b);
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqint(rb_read_until(&b, tmp, 8, "ed", 2, &tmplen), RB_ERR_NOTFOUND);
    eqint(tmplen, 7); 
    eqnstr(tmp, "abcdefg", 7);
    eqint(b.writer, 7);
    eqint(b.reader, 0);
    eqint(b.writecounter, 7);

    eqint(rb_read_until(&b, tmp, 8, "fg", 2, &tmplen), RB_OK);
    eqint(tmplen, 7); 
    
    eqint(rb_read_until(&b, tmp, 8, "yz", 2, &tmplen), RB_ERR_NOTFOUND);
    eqint(tmplen, 0); 

}


void test_read_until_chr() {
    char tmp[256];
    size16_t tmplen = 0;
    char buff[S];
    struct ringbuffer b;
    rb_init(&b, buff, S);

    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqint(rb_read_until_chr(&b, tmp, 7, 'd', &tmplen), RB_OK);
    eqint(tmplen, 4); 
    eqnstr(tmp, "abcd", 4);
    eqint(b.writer, 7);
    eqint(b.reader, 4);
    eqint(b.writecounter, 7);

    tmplen = 0;
    RB_RESET(&b);
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqint(rb_read_until_chr(&b, tmp, 8, 'z', &tmplen), RB_ERR_NOTFOUND);
    eqint(tmplen, 7); 
    eqnstr(tmp, "abcdefg", 7);
    eqint(b.writer, 7);
    eqint(b.reader, 0);
    eqint(b.writecounter, 7);

    tmplen = 0;
    RB_RESET(&b);
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqint(rb_read_until_chr(&b, tmp, 4, 'z', &tmplen), RB_ERR_NOTFOUND);
    eqint(tmplen, 4); 
    eqnstr(tmp, "abcd", 4);
    eqint(b.writer, 7);
    eqint(b.reader, 0);
    eqint(b.writecounter, 7);

    eqint(rb_read_until_chr(&b, tmp, 7, 'g', &tmplen), RB_OK);
    eqint(tmplen, 7); 
    eqint(rb_read_until_chr(&b, tmp, 7, 'z', &tmplen), RB_ERR_NOTFOUND);
    eqint(tmplen, 0); 
} 


void test_dryread_until() {
    char tmp[256];
    size16_t tmplen = 0;
    char buff[S];
    struct ringbuffer b;
    rb_init(&b, buff, S);

    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqint(rb_dryread_until(&b, tmp, 7, "de", 2, &tmplen), RB_OK);
    eqint(tmplen, 5); 
    eqnstr(tmp, "abc", 3);
    eqint(b.writer, 7);
    eqint(b.reader, 0);
    eqint(b.writecounter, 7);

    tmplen = 0;
    RB_RESET(&b);
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqint(rb_dryread_until(&b, tmp, 7, "ed", 2, &tmplen), RB_ERR_NOTFOUND);
    eqint(tmplen, 7); 
    eqnstr(tmp, "abcdefg", 7);
    eqint(b.writer, 7);
    eqint(b.reader, 0);
    eqint(b.writecounter, 7);

    tmplen = 0;
    RB_RESET(&b);
    eqint(rb_write(&b, "abcdefg", 7), RB_OK);
    eqint(rb_dryread_until(&b, tmp, 8, "ed", 2, &tmplen), RB_ERR_NOTFOUND);
    eqint(tmplen, 7); 
    eqnstr(tmp, "abcdefg", 7);
    eqint(b.writer, 7);
    eqint(b.reader, 0);
    eqint(b.writecounter, 7);

    eqint(rb_read_until(&b, tmp, 8, "fg", 2, &tmplen), RB_OK);
    eqint(tmplen, 7); 

    eqint(rb_dryread_until(&b, tmp, 8, "yz", 2, &tmplen), RB_ERR_NOTFOUND);
    eqint(tmplen, 0); 
}


/* Return space available up to the end of the buffer.  */
#define CIRC_SPACE_TO_END(writer,reader,size) \
	({int end = (size) - 1 - (writer); \
	  int n = (end + (reader)) & ((size)-1); \
	  n <= end ? n : end+1;})


int main() {
    test_write_read();
    test_dryread();
    test_dryread_until();
    test_read_until();
    test_read_until_chr();
//    
//    int i = CIRC_SPACE_TO_END(2, 6, 8);
//    printf("Consecutive space to end: %d\n", i);
}

