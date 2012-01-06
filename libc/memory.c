#include <memory.h>
#include <stdint.h>

void memcopy(uint8_t *dst, uint8_t *src, uint32_t len)
{
	for(;len;len--) *dst++ = *src++;
}

void memset(uint8_t *dst, uint8_t val, uint32_t len)
{
	for(;len;len--) *dst++ = val;
}
