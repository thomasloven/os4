#pragma once

#include <stdint.h>
#include "filp.h"
#include "vnode.h"

#define NUM_FILP 64
#define PROC_NUM 256

typedef struct fproc_struct
{
	vnode_t *wd;
	vnode_t *rd;
	filp_t *filp[NUM_FILP];
	uint32_t pid;
} fproc_t;

void init_fproc();
fproc_t *new_fproc(uint32_t pid, uint32_t ppid);
fproc_t *get_fproc(uint32_t pid);
void kill_fproc(uint32_t pid);