#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <stdint.h>

typedef volatile uint32_t semaphore;

extern void spin(semaphore*);



#endif