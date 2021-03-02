
#include "ringbuffer.h"


//#define rb_increment_writer(rb) (rb)->writer = rb_writer_next(rb)
//#define rb_calc(rb, i, s) (((s) + (i)) % (rb)->size)
//#define rb_used(rb) (((rb)->reader > (rb)->writer? (rb)->size: 0) + (rb)->writer - (rb)->reader)
//#define rb_available(rb) ((rb)->size - rb_used(rb) - 1)

#define rb_writer_next(rb) ((1 + (rb)->writer) % (rb)->size)
#define rb_reader_next(rb) ((1 + (rb)->reader) % (rb)->size)
#define rb_increment_reader(rb) (rb)->reader = rb_reader_next(rb)


FUNC_ATTR
rberr_t rb_pushone(struct ringbuffer *b, char byte) {
    uint16_t writernext = rb_writer_next(b);
    if (writernext == b->reader) {
        switch (b->overflow) {
            case RB_OVERFLOW_ERROR:
                return RB_ERR_FULL;
            case RB_OVERFLOW_IGNORE_OLDER:
                /* Ignore one byte and forward reader's needle one step */
                rb_increment_reader(b);
                break;
            case RB_OVERFLOW_IGNORE_NEWER:
                /* Ignore the newly received byte */
                return RB_OK;
        }
    }

    b->blob[b->writer] = byte;
    b->writer = writernext;
    return RB_OK;
}


FUNC_ATTR
rberr_t rb_push(struct ringbuffer *b, char *data, uint16_t len) {
    uint16_t i;
    rberr_t err;
    for(i = 0; i < len; i++) {
        err = rb_pushone(b, data[i]);
        if (err) {
            return err;
        }
    }
    return RB_OK;
}


FUNC_ATTR
void rb_init(struct ringbuffer *b, char *buff, uint16_t size,
        enum rb_overflow overflow) {
    b->size = size;
    b->reader = 0;
    b->writer = 0;
    b->blob = buff;
    b->overflow = overflow;
}
/*
ICACHE_FLASH_ATTR
void rb_pop(RingBuffer *rb, char *data, uint16_t datalen) {
    uint16_t i;
    for (i = 0; i < datalen; i++) {
        data[i] = rb->blob[rb->reader];
        rb_increment(rb, rb->reader, 1); 
    }
}


ICACHE_FLASH_ATTR
int rb_safepush(RingBuffer *rb, char *data, uint16_t datalen) {
    if (rb_canpush(rb, datalen)) {
        rb_push(rb, data, datalen);
        return RB_OK;
    }
    return RB_FULL;
}


ICACHE_FLASH_ATTR
int rb_safepop(RingBuffer *rb, char *data, uint16_t datalen) {
    if (rb_canpop(rb, datalen)) {
        rb_pop(rb, data, datalen);
        return RB_OK;
    }
    return RB_INSUFFICIENT;
}


ICACHE_FLASH_ATTR
void rb_drypop(RingBuffer *rb, char *data, uint16_t datalen) {
    uint16_t i;
    for (i = 0; i < datalen; i++) {
        data[i] = rb->blob[rb_calc(rb, rb->reader, i)];
    }
}


ICACHE_FLASH_ATTR
void rb_reset(RingBuffer *rb) {
    rb->reader = 0;
    rb->writer = 0;
}
*/
