
#include "ringbuffer.h"


//#define rb_increment_writer(rb) (rb)->writer = rb_writer_next(rb)
//#define rb_calc(rb, i, s) (((s) + (i)) % (rb)->size)


FUNC_ATTR
rberr_t rb_pushone(struct ringbuffer *b, char byte) {
    uint16_t writernext = rb_writer_calc(b, 1);
    if (writernext == b->reader) {
        switch (b->overflow) {
            case RB_OVERFLOW_ERROR:
                return RB_ERR_INSUFFICIENT;
            case RB_OVERFLOW_IGNORE_OLDER:
                /* Ignore one byte and forward reader's needle one step */
                rb_reader_skip(b, 1);
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
        rb_reader_skip(b, 1);
        if (b->reader == b->writer) {
            return i + 1;
        }
    }
    return len;
}


FUNC_ATTR
uint16_t rb_dryread(struct ringbuffer *b, char *data, uint16_t len) {
    uint16_t i;
    uint16_t n;
    for (i = 0; i < len; i++) {
        n = rb_reader_calc(b, i);
        if (n == b->writer) {
            return i;
        }
        data[i] = b->blob[n];
    }
    return len;
}

