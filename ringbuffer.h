#ifndef RINGBUFFER_H
#define RINGBUFFER_H


/* Platform Specific Options */
#include <stdint.h>  // For uintx_t

#define FUNC_ATTR   
// TODO: ICACHE_FLASH_ATTR


#define RB_OK                0
#define RB_ERR_INSUFFICIENT     -1


#define RB_CALC(b, n)         ((n) & ((b)->size - 1))
#define RB_USED(b)            RB_CALC(b, (b)->writer - (b)->reader)
#define RB_AVAILABLE(b)       RB_CALC(b, (b)->reader - ((b)->writer + 1))
#define RB_WRITER_MOVE(b, n)  RB_CALC(b, (b)->writer + (n))
#define RB_READER_MOVE(b, n)  RB_CALC(b, (b)->reader + (n))
#define RB_READER_SKIP(b, n)  (b)->reader = RB_READER_MOVE(b, n)
#define RB_RESET(b) (b)->reader = (b)->writer = 0


typedef signed char rberr_t; 
typedef unsigned int rbsize_t; 


enum rb_overflow {
    RB_OVERFLOW_ERROR,
    RB_OVERFLOW_IGNORE_OLDER,
    RB_OVERFLOW_IGNORE_NEWER
};


struct ringbuffer{
    uint16_t size;
    int reader;
    int writer;
    char *blob;
    enum rb_overflow overflow;
};


rbsize_t rb_read(struct ringbuffer *b, char *data, uint16_t len);
rbsize_t rb_dryread(struct ringbuffer *b, char *data, uint16_t len);

rberr_t rb_pushone(struct ringbuffer *rb, char byte);
rberr_t rb_write(struct ringbuffer *b, char *data, uint16_t len);
void rb_init(struct ringbuffer *b, char *buff, uint16_t size, 
        enum rb_overflow overflow);

#endif

