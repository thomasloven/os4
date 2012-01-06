#pragma once

#include <vfs.h>

#include "fs.h"

void init_inodes();
void add_node(inode_t *node, inode_t *new, char *name);
inode_t *new_inode(inode_t *innode);
