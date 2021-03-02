/*
 * ringbuf.c
 *
 * copyright (c) 2020-2021 Xiongfei Shi
 *
 * author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 * license: Apache-2.0
 *
 * https://github.com/shixiongfei/ringbuf
 */

#include "ringbuf.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void *alloc_emul(void *ptr, size_t size) {
  if (size)
    return realloc(ptr, size);
  free(ptr);
  return NULL;
}

static void *(*rb_realloc)(void *, size_t) = alloc_emul;

#define rb_malloc(size) rb_realloc(NULL, size)
#define rb_free(ptr) rb_realloc(ptr, 0)

static void dummy(void *lock) { (void)lock; }

static void (*rb_lock)(void *) = dummy;
static void (*rb_unlock)(void *) = dummy;

void ringbuf_setalloc(void *(*allocator)(void *, size_t)) {
  rb_realloc = allocator ? allocator : alloc_emul;
}
void ringbuf_setlock(void (*acquire)(void *), void (*release)(void *)) {
  rb_lock = acquire ? acquire : dummy;
  rb_unlock = release ? release : dummy;
}

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

int ringbuf_create(ringbuf_t *rb, int size, unsigned int flags, void *lock) {
  assert(size > 0);

  if (0 != (size & (size - 1)))
    return -1;

  rb->lock = lock;
  rb->flags = flags;
  rb->size = size;
  rb->mask = rb->size - 1;
  rb->read_pos = 0;
  rb->write_pos = 0;
  rb->buffer = (unsigned char *)rb_malloc(rb->size);

  return 0;
}

void ringbuf_destroy(ringbuf_t *rb) {
  if (rb->buffer)
    rb_free(rb->buffer);
}

void ringbuf_reset(ringbuf_t *rb) {
  rb_lock(rb->lock);
  rb->read_pos = 0;
  rb->write_pos = 0;
  rb_unlock(rb->lock);
}

int ringbuf_capacity(ringbuf_t *rb) { return rb->size; }

int ringbuf_size(ringbuf_t *rb) {
  int size;

  rb_lock(rb->lock);
  size = (int)(rb->write_pos - rb->read_pos);
  rb_unlock(rb->lock);

  return size;
}

int ringbuf_usable(ringbuf_t *rb) {
  return ringbuf_capacity(rb) - ringbuf_size(rb);
}

int ringbuf_read(ringbuf_t *rb, void *buffer, int size) {
  int total, len, part;

  rb_lock(rb->lock);

  total = (int)(rb->write_pos - rb->read_pos);
  len = min(size, total);
  part = min(len, (int)(rb->size - (rb->read_pos & rb->mask)));

  memcpy(buffer, rb->buffer + (rb->read_pos & rb->mask), part);
  memcpy((unsigned char *)buffer + part, rb->buffer, len - part);

  rb->read_pos += len;
  rb_unlock(rb->lock);

  return len;
}

static int ringbuf_expandifneeded(ringbuf_t *rb, int len) {
  int total, size, part, rest;
  unsigned char *buffer;

  size = rb->size;
  rest = (int)(size - rb->write_pos + rb->read_pos);

  if (rest > len)
    return 0;

  if (RB_EXTENDABLE != (rb->flags & RB_EXTENDABLE))
    return -1;

  do {
    size = size << 1;
  } while (size < (int)(rb->size + len));

  buffer = (unsigned char *)rb_malloc(size);

  total = (int)(rb->write_pos - rb->read_pos);
  part = min(total, (int)(rb->size - (rb->read_pos & rb->mask)));

  memcpy(buffer, rb->buffer + (rb->read_pos & rb->mask), part);
  memcpy((unsigned char *)buffer + part, rb->buffer, total - part);

  rb_free(rb->buffer);
  rb->buffer = buffer;
  rb->size = size;
  rb->mask = rb->size - 1;
  rb->read_pos = 0;
  rb->write_pos = total;

  return 0;
}

int ringbuf_write(ringbuf_t *rb, const void *buffer, int size) {
  int part;

  rb_lock(rb->lock);

  if (0 != ringbuf_expandifneeded(rb, size)) {
    rb_unlock(rb->lock);
    return 0;
  }

  size = min(size, (int)(rb->size - rb->write_pos + rb->read_pos));
  part = min(size, (int)(rb->size - (rb->write_pos & rb->mask)));

  memcpy(rb->buffer + (rb->write_pos & rb->mask), buffer, part);
  memcpy(rb->buffer, (unsigned char *)buffer + part, size - part);

  rb->write_pos += size;
  rb_unlock(rb->lock);

  return size;
}
