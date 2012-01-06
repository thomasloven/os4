#ifndef KDEBUG_H
#define KDEBUG_H

#include <debug.h>

#define VIDMEM 0xC00B8000

#define SCRN_W 80
#define SCRN_H 25

void init_debug();
void scroll();
void k_putch(char c, uint8_t style, uint32_t pos);
void k_printf(char *str, ...);
int k_num2str(uint32_t num, uint32_t base, char *buf);
void setColor(uint32_t clr);
#endif