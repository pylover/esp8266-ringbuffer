#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#ifdef TESTING
typedef char httpd_err_t;
typedef unsigned short size16_t;
typedef unsigned int size32_t;
typedef unsigned int uint32_t;
#else
#include "httpdtypes.h"
#endif

#define RB_OK                    0
#define RB_ERR_INSUFFICIENT     -100
#define RB_ERR_NOTFOUND         -101


#define RB_CALC(b, n)         ((n) & ((b)->size - 1))
#define RB_USED(b)            RB_CALC(b, (b)->writer - (b)->reader)
#define RB_AVAILABLE(b)       RB_CALC(b, (b)->reader - ((b)->writer + 1))
#define RB_WRITER_CALC(b, n)  RB_CALC(b, (b)->writer + (n))
#define RB_READER_CALC(b, n)  RB_CALC(b, (b)->reader + (n))
#define RB_READER_SKIP(b, n)  (b)->reader = RB_READER_CALC((b), (n))
#define RB_RESET(b) ({ \
    (b)->writecounter = 0; \
    (b)->reader = 0; \
    (b)->writer = 0; \
})


#define RB_USED_TOEND(b) \
	({int end = ((b)->size) - ((b)->reader); \
	  int n = (((b)->writer) + end) & (((b)->size)-1); \
	  n < end ? n : end;})

#define RB_FREE_TOEND(b) \
	({int end = ((b)->size) - 1 - ((b)->writer); \
	  int n = (end + ((b)->reader)) & (((b)->size)-1); \
	  n <= end ? n : end+1;})



struct ringbuffer{
    size16_t size;
    int reader;
    int writer;
    uint32_t writecounter;
    char *blob;
};


httpd_err_t rb_read_until_chr(struct ringbuffer *b, char *data, size16_t len,
        char delimiter, size16_t *readlen);
httpd_err_t rb_read_until(struct ringbuffer *b, char *data, size16_t len,
        char *delimiter, size16_t dlen, size16_t *readlen);
size16_t rb_read(struct ringbuffer *b, char *data, size16_t len);
size16_t rb_dryread(struct ringbuffer *b, char *data, size16_t len);
httpd_err_t rb_dryread_until(struct ringbuffer *b, char *data, size16_t len,
        char *delimiter, size16_t dlen, size16_t *readlen);
httpd_err_t rb_pushone(struct ringbuffer *rb, char byte);
httpd_err_t rb_write(struct ringbuffer *b, char *data, size16_t len);
void rb_init(struct ringbuffer *b, char *buff, size16_t size);

#endif

