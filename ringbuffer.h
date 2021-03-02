#ifndef RINGBUFFER_H
#define RINGBUFFER_H


/* Platform Specific Options */
#include <stdint.h>  // For uintx_t

#define FUNC_ATTR   
// TODO: ICACHE_FLASH_ATTR


#define RB_OK                0
#define RB_ERR_INSUFFICIENT     -1

//#define CIRC_CNT(head,tail,size) (((head) - (tail)) & ((size)-1))

#define rb_calc(rb, f, n) (((n) + (f)) & ((rb)->size - 1))

#define rb_used(rb) (((rb)->reader > (rb)->writer? (rb)->size: 0) + \
        (rb)->writer - (rb)->reader)
#define rb_available(rb) ((rb)->size - rb_used(rb) - 1)

#define rb_writer_calc(rb, n) rb_calc(rb, (rb)->writer, n)
#define rb_reader_calc(rb, n) rb_calc(rb, (rb)->reader, n)

#define rb_reset(rb) (rb)->reader = (rb)->writer = 0
#define rb_reader_skip(rb, n) (rb)->reader = rb_reader_calc(rb, n)


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

