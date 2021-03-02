#ifndef RINGBUFFER_H
#define RINGBUFFER_H


/* Platform Specific Options */
#include <stdint.h>  // For uintx_t

#define FUNC_ATTR   
// TODO: ICACHE_FLASH_ATTR


#define RB_OK                0
#define RB_ERR_INSUFFICIENT     -1


#define rb_reset(rb) (rb)->reader = (rb)->writer = 0
#define rb_used(rb) (((rb)->reader > (rb)->writer? (rb)->size: 0) + \
        (rb)->writer - (rb)->reader)

#define rb_available(rb) ((rb)->size - rb_used(rb) - 1)



typedef signed char rberr_t; 


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


uint16_t rb_read(struct ringbuffer *b, char *data, uint16_t len);

rberr_t rb_pushone(struct ringbuffer *rb, char byte);
rberr_t rb_write(struct ringbuffer *b, char *data, uint16_t len);
void rb_init(struct ringbuffer *b, char *buff, uint16_t size, 
        enum rb_overflow overflow);

/*
void rb_write(RingBuffer *rb, char *data, uint16_t datalen);
void rb_readone(RingBuffer *rb, char *data);
void rb_read(RingBuffer *rb, char *data, uint16_t datalen);
void rb_dryread(RingBuffer *rb, char *data, uint16_t datalen);
void rb_skip(RingBuffer *rb, uint16_t datalen);
int rb_safewrite(RingBuffer *rb, char *data, uint16_t datalen);
int rb_saferead(RingBuffer *rb, char *data, uint16_t datalen);
void rb_reset(RingBuffer *rb);

#define rb_skip(rb, s) rb_increment(rb, (rb)->reader, s)
#define rb_canwrite(rb, len) (len <= rb_available(rb))
#define rb_canread(rb, len) (len <= rb_used(rb))
*/
#endif

