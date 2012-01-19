#pragma once

#include <stdint.h>
#include "vnode.h"

typedef struct filp_struct
{
	uint32_t mode;
	uint32_t flags;
	uint32_t use_counter;
	vnode_t *vnode;
	uint32_t offset;
} filp_t;

filp_t *new_filp();
void dup_filp(filp_t *filp);
void put_filp(filp_t *filp);
void drop_filp(filp_t *filp);