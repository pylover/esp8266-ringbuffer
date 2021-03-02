#include "ringbuffer.h"


FUNC_ATTR
rberr_t rb_pushone(struct ringbuffer *b, char byte) {
    rb_size_t writernext = RB_WRITER_MOVE(b, 1);
    if (writernext == b->reader) {
        switch (b->overflow) {
            case RB_OVERFLOW_ERROR:
                return RB_ERR_INSUFFICIENT;
            case RB_OVERFLOW_IGNORE_OLDER:
                /* Ignore one byte and forward reader's needle one step */
                RB_READER_SKIP(b, 1);
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
rberr_t rb_write(struct ringbuffer *b, char *data, rb_size_t len) {
    rb_size_t i;
    
    if ((b->overflow == RB_OVERFLOW_ERROR) && (RB_AVAILABLE(b) < len)) {
        return RB_ERR_INSUFFICIENT;
    }

    for(i = 0; i < len; i++) {
        rb_pushone(b, data[i]);
    }
    return RB_OK;
}


FUNC_ATTR
void rb_init(struct ringbuffer *b, char *buff, rb_size_t size,
        enum rb_overflow overflow) {
    b->size = size;
    b->reader = 0;
    b->writer = 0;
    b->blob = buff;
    b->overflow = overflow;
}


FUNC_ATTR
rb_size_t rb_read(struct ringbuffer *b, char *data, rb_size_t len) {
    rb_size_t i;
    for (i = 0; i < len; i++) {
        if (b->reader == b->writer) {
            return i;
        }
        data[i] = b->blob[b->reader];
        RB_READER_SKIP(b, 1);
    }
    return len;
}


FUNC_ATTR
rb_size_t rb_dryread(struct ringbuffer *b, char *data, rb_size_t len) {
    rb_size_t i;
    rb_size_t n;
    for (i = 0; i < len; i++) {
        n = RB_READER_MOVE(b, i);
        if (n == b->writer) {
            return i;
        }
        data[i] = b->blob[n];
    }
    return len;
}

