#pragma once

#include <stdint.h>
#include <vfs.h>
#include "fs.h"

dirent_t *fs_readdir(inode_t *node, uint32_t num);

// May need to put a mutex lock on this
dirent_t dirent;