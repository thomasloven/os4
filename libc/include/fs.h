#ifndef FS_H
#define FS_H

#include <stdint.h>

#define FS_FILE	0x1
#define FS_DIRECTORY	0x2
#define FS_CHARDEVICE	0x3
#define FS_BLOCKDEVICE	0x4
#define FS_PIPE	0x5
#define FS_SYMLINK	0x6
#define FS_MOUNT	0x8

typedef uint32_t FILE;





#endif