
#include "ringbuffer.h"


//#define rb_increment_writer(rb) (rb)->writer = rb_writer_next(rb)
//#define rb_calc(rb, i, s) (((s) + (i)) % (rb)->size)

#define rb_writer_next(rb) ((1 + (rb)->writer) % (rb)->size)
#define rb_reader_next(rb) ((1 + (rb)->reader) % (rb)->size)
#define rb_increment_reader(rb) (rb)->reader = rb_reader_next(rb)

FUNC_ATTR
rberr_t rb_pushone(struct ringbuffer *b, char byte) {
    uint16_t writernext = rb_writer_next(b);
    if (writernext == b->reader) {
        switch (b->overflow) {
            case RB_OVERFLOW_ERROR:
                return RB_ERR_INSUFFICIENT;
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
rberr_t rb_write(struct ringbuffer *b, char *data, uint16_t len) {
    uint16_t i;
    
    if ((b->overflow == RB_OVERFLOW_ERROR) && (rb_available(b) < len)) {
        return RB_ERR_INSUFFICIENT;
    }

    for(i = 0; i < len; i++) {
        rb_pushone(b, data[i]);
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


FUNC_ATTR
uint16_t rb_read(struct ringbuffer *b, char *data, uint16_t len) {
    uint16_t i;
    for (i = 0; i < len; i++) {
        data[i] = b->blob[b->reader];
        rb_increment_reader(b);
        if (b->reader == b->writer) {
            return i + 1;
        }
    }
    return len;
}


/*


ICACHE_FLASH_ATTR
void rb_dryread(RingBuffer *rb, char *data, uint16_t datalen) {
    uint16_t i;
    for (i = 0; i < datalen; i++) {
        data[i] = rb->blob[rb_calc(rb, rb->reader, i)];
    }
}


*/
