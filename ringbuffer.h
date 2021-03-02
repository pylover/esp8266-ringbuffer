#ifndef RINGBUFFER_H
#define RINGBUFFER_H


/* Platform Specific Options */
#include <stdint.h>  // For uintx_t

#define FUNC_ATTR   
// TODO: ICACHE_FLASH_ATTR


#define RB_OK                0
#define RB_ERR_FULL             -1
#define RB_ERR_INSUFFICIENT     -2


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


rberr_t rb_pushone(struct ringbuffer *rb, char byte);
rberr_t rb_push(struct ringbuffer *b, char *data, uint16_t len);
void rb_init(struct ringbuffer *b, char *buff, uint16_t size, 
        enum rb_overflow overflow);

/*
void rb_push(RingBuffer *rb, char *data, uint16_t datalen);
void rb_popone(RingBuffer *rb, char *data);
void rb_pop(RingBuffer *rb, char *data, uint16_t datalen);
void rb_drypop(RingBuffer *rb, char *data, uint16_t datalen);
void rb_skip(RingBuffer *rb, uint16_t datalen);
int rb_safepush(RingBuffer *rb, char *data, uint16_t datalen);
int rb_safepop(RingBuffer *rb, char *data, uint16_t datalen);
void rb_reset(RingBuffer *rb);

#define rb_skip(rb, s) rb_increment(rb, (rb)->reader, s)
#define rb_canpush(rb, len) (len <= rb_available(rb))
#define rb_canpop(rb, len) (len <= rb_used(rb))
*/
#endif

