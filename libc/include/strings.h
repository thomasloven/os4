#ifndef STRINGS_H
#define STRINGS_H

#include <stdint.h>

uint32_t strcmp(char *str1, char *str2);
void strcopy(char *dst, char *src);
uint32_t strlen(char *str);
int num2str(uint32_t num, uint32_t base, char *buf);

#endif
