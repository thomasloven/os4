#ifndef KIPC_H
#define KIPC_H

#include <stdint.h>


int32_t pull(uint8_t *dst, uint8_t *src, uint32_t proc, uint32_t length);
int32_t push(uint32_t proc, uint8_t *dst, uint8_t *src, uint32_t length);

#endif