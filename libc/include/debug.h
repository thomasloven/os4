#ifndef DEBUG_H
#define DEBUG_H

#define BREAK asm volatile ("xchg %bx, %bx;");

#endif
