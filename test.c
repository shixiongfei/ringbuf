/*
 * test.c
 *
 * copyright (c) 2020-2021 Xiongfei Shi
 *
 * author: Xiongfei Shi <xiongfei.shi(a)icloud.com>
 * license: Apache-2.0
 *
 * https://github.com/shixiongfei/ringbuf
 */

#include "ringbuf.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  ringbuf_t rb;
  char buffer[16] = {0};

  ringbuf_create(&rb, 8, RB_EXTENDABLE, NULL);

  printf("W: %d\n", ringbuf_write(&rb, "Hello", 5));
  printf("W: %d\n", ringbuf_write(&rb, "World", 5));
  printf("C: %d\n", ringbuf_capacity(&rb));
  printf("S: %d\n", ringbuf_size(&rb));
  printf("U: %d\n", ringbuf_usable(&rb));
  printf("R: %d\n", ringbuf_read(&rb, buffer, 16));
  printf("D: %s\n", buffer);

  ringbuf_destroy(&rb);
  return 0;
}
