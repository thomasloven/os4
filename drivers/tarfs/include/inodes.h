#pragma once

#include <vfs.h>

#include "fs.h"
#include "tar.h"

void init_inodes();
void add_node(inode_t *node, inode_t *new, char *name);
inode_t *new_inode(tar_head *head);

inode_t *get_inode(uint32_t inode_num);
tar_head *get_inode_tar(uint32_t inode_num);