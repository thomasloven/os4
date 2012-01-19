#pragma once

#include <stdint.h>
#include <vfs.h>
#include "fs.h"
#include "tar.h"

void tarfs_getnode(vfsm *msg, uint32_t sender, tar_head *files);
void tarfs_readdir(vfsm *msg, uint32_t sender);
void tarfs_lookup(vfsm *message, uint32_t sender);
dirent_t *fs_readdir(inode_t *node, uint32_t num);
uint32_t fs_finddir(inode_t *node, char *name);

// May need to put a mutex lock on this
dirent_t dirent;