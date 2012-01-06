#ifndef IPC_H
#define IPC_H

typedef struct signal_struct signal_t; 

#include <stdint.h>

#define NUM_SIGNALS 256

#define SIG_IGN	1
#define SIG_DFL	0

//#define SIGSYNC	0x1
#define SIGIRQ	0x2
#define SIGACK	0x3
#define SIGTERM	0x4
#define SIGKILL	0x9

#define SIGMSG	0x10
#define SIGMAC	0x11
#define SIGSYNC	0x12

typedef struct signal_struct
{
	uint32_t	signum;
	uint32_t	sender;
	signal_t	*next;
} signal_t;

typedef struct signal_queue_struct
{
	volatile signal_t *first;
	volatile signal_t *last;
} signal_queue;

typedef void (*sig_handler)(uint32_t signum, uint32_t sender);

void _event(uint32_t sender, uint32_t signum);
void signal(uint32_t signum, uint32_t *handler, uint32_t *old);
void kill(uint32_t pid, uint32_t signum);
void signals_init();
void signal_buffer(uint32_t signum, uint32_t sender);
int32_t sigpending(uint32_t signum, uint32_t sender);
int32_t waitsig(uint32_t signum,uint32_t pid);
void send(uint32_t pid, uint32_t length, uint8_t *message);
uint32_t recieve(uint32_t pid, uint8_t *buffer);
int32_t msglen(uint32_t pid);
int32_t msgsrc();
void *waitmsg(uint32_t pid, uint32_t *sender);

#define send2(a,b) send(a, sizeof(*b), (uint8_t *)b)

#endif
