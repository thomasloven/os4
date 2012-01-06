#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

void memcopy(uint8_t *dst, uint8_t *src, uint32_t len);
void memset(uint8_t *dst, uint8_t val, uint32_t len);

#endif