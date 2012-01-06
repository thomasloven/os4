#pragma once

#include <stdint.h>
#include "filp.h"
#include "vnode.h"

#define NUM_FILP 64

typedef struct fproc_struct
{
	vnode_t *wd;
	vnode_t *rd;
	filp_t filp[NUM_FILP];
	uint32_t pid;
} fproc_t;