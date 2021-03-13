#include "ringbuffer.h"

#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <string.h>
#include <osapi.h>

#define MIN(x, y) (((x) > (y))? (y): (x))


ICACHE_FLASH_ATTR
httpd_err_t rb_write(struct ringbuffer *b, char *data, size16_t len) {
    
    if (RB_AVAILABLE(b) < len) {
        return RB_ERR_INSUFFICIENT;
    }
    
    size16_t toend = RB_FREE_TOEND(b);
    size16_t chunklen = MIN(toend, len);
    os_memcpy(b->blob + b->writer, data, chunklen);
    b->writer = RB_WRITER_CALC(b, chunklen);
    b->writecounter += chunklen;

    if (len > chunklen) {
        len -= chunklen;
        os_memcpy(b->blob, data + chunklen, len);
        b->writer += len;
        b->writecounter += len;
    }
    return RB_OK;
}


ICACHE_FLASH_ATTR
size16_t rb_read(struct ringbuffer *b, char *data, size16_t len) {
    size16_t toend = RB_USED_TOEND(b);
    size16_t total = MIN(toend, len);
    os_memcpy(data, b->blob + b->reader, total);
    b->reader = RB_READER_CALC(b, total);
    len -= total;

    if (len) {
        len = MIN(RB_USED_TOEND(b), len);
        os_memcpy(data + total, b->blob, len);
        b->reader += len;
        total += len;
    }
   
    return total;
}


ICACHE_FLASH_ATTR
size16_t rb_dryread(struct ringbuffer *b, char *data, size16_t len) {
    size16_t toend = RB_USED_TOEND(b);
    size16_t total = MIN(toend, len);
    os_memcpy(data, b->blob + b->reader, total);
    len -= total;

    if (len) {
        len = MIN(RB_USED(b) - total, len);
        os_memcpy(data + total, b->blob, len);
        total += len;
    }
    return total;
}


ICACHE_FLASH_ATTR
httpd_err_t rb_read_until(struct ringbuffer *b, char *data, size16_t len,
        char *delimiter, size16_t dlen, size16_t *readlen) {
    size16_t rl = rb_dryread(b, data, len);
    char *f = memmem(data, rl, delimiter, dlen);
    if (f == NULL) {
        return RB_ERR_NOTFOUND;
    }
    *readlen = (f - data) + dlen;
    b->reader = RB_READER_CALC(b, *readlen);
    return RB_OK;
}


ICACHE_FLASH_ATTR
httpd_err_t rb_dryread_until(struct ringbuffer *b, char *data, size16_t len,
        char *delimiter, size16_t dlen, size16_t *readlen) {
    size16_t rl = rb_dryread(b, data, len);
    char *f = memmem(data, rl, delimiter, dlen);
    if (f == NULL) {
        return RB_ERR_NOTFOUND;
    }
    *readlen = (f - data) + dlen;
    return RB_OK;
}


ICACHE_FLASH_ATTR
httpd_err_t rb_read_until_chr(struct ringbuffer *b, char *data, size16_t len,
        char delimiter, size16_t *readlen) {

    size16_t rl = rb_dryread(b, data, len);
    char *f = memchr(data, delimiter, rl);
    if (f == NULL) {
        return RB_ERR_NOTFOUND;
    }
    *readlen = (f - data) + 1;
    b->reader = RB_READER_CALC(b, *readlen);
    return RB_OK;
}


ICACHE_FLASH_ATTR
void rb_init(struct ringbuffer *b, char *buff, size16_t size) {
    b->size = size;
    b->reader = 0;
    b->writer = 0;
    b->writecounter = 0;
    b->blob = buff;
}



