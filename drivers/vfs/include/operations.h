#pragma once

#include <stdint.h>
#include <vfs.h>
#include "vnode.h"
#include "fproc.h"
#include "filp.h"

void vfs_fork(vfsm *message, uint32_t sender);
void vfs_chrd(vfsm *message, uint32_t sender);
void vfs_chwd(vfsm *message, uint32_t sender);
void vfs_readdir(vfsm *message, uint32_t sender);
