/*
 * ringbuf.h
 *
 * copyright (c) 2020-2021 Xiongfei Shi
 *
 * author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 * license: Apache-2.0
 *
 * https://github.com/shixiongfei/ringbuf
 */

#ifndef __RINGBUF_H__
#define __RINGBUF_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RB_EXTENDABLE (1 << 0)

typedef struct ringbuf_t {
  void *lock;
  unsigned char *buffer;
  unsigned int flags;
  unsigned int size;
  unsigned int mask;
  unsigned int read_pos;
  unsigned int write_pos;
} ringbuf_t;

void ringbuf_setalloc(void *(*allocator)(void *, size_t));
void ringbuf_setlock(void (*acquire)(void *), void (*release)(void *));

int ringbuf_create(ringbuf_t *rb, int size, unsigned int flags, void *lock);
void ringbuf_destroy(ringbuf_t *rb);

void ringbuf_reset(ringbuf_t *rb);
int ringbuf_capacity(ringbuf_t *rb);
int ringbuf_size(ringbuf_t *rb);
int ringbuf_usable(ringbuf_t *rb);

int ringbuf_read(ringbuf_t *rb, void *buffer, int size);
int ringbuf_write(ringbuf_t *rb, const void *buffer, int size);

#ifdef __cplusplus
};
#endif

#endif /* __RINGBUF_H__ */
