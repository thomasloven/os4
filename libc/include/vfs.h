#ifndef VFS_H
#define VFS_H

#include <stdint.h>


#define VMSG_READ	0x11
#define VMSG_WRITE	0x12
#define VMSG_OPEN	0x13
#define VMSG_CLOSE	0x14
#define VMSG_READDIR	0x15
#define VMSG_FINDDIR	0x16
#define VMSG_MOUNT	0x17
#define VMSG_TOUCH	0x18
#define VMSG_REPLY 0x20


#define FILE_NAME_LENGTH 128

#define VFS_FILE		0x1
#define VFS_DIRECTORY	0x2
#define VFS_CHARDEVICE	0x3
#define VFS_BLOCKDEVICE	0x4
#define VFS_PIPE		0x5
#define VFS_SYMLINK		0x6
#define VFS_MOUNTPOINT	0x8

typedef struct vfs_inode_struct
{
	uint32_t driver_num;
	uint32_t inode_number;
	uint32_t flags;
	uint32_t use_counter;
} inode_t;

typedef struct dirent_struct
{
	char name[FILE_NAME_LENGTH];
	uint32_t inode_number;
} dirent_t;

typedef struct vfs_msg_struct
{
	uint32_t msg_type;
} vfs_msg;





typedef struct vfs_msg_read_struct
{
	uint32_t msg_type;
	inode_t node;
	uint32_t length;
	uint32_t offset;
} vfs_msg_read;

typedef struct vfs_msg_read_reply_struct
{
	uint32_t msg_type;
	uint32_t length;
	uint8_t data;
} vfs_msg_read_reply;

typedef struct vfs_msg_write_struct
{
	uint32_t msg_type;
	inode_t node;
	uint32_t length;
	uint32_t offset;
	uint8_t data;
} vfs_msg_write;

typedef struct vfs_msg_write_reply_struct
{
	uint32_t msg_type;
	uint32_t length;
} vfs_msg_write_reply;

typedef struct vfs_msg_open_struct
{
	uint32_t msg_type;
	inode_t node;
} vfs_msg_open;

typedef vfs_msg vfs_msg_open_reply;

typedef struct vfs_msg_close_struct
{
	uint32_t msg_type;
	inode_t node;
} vfs_msg_close;

typedef vfs_msg vfs_msg_open_reply;

typedef struct vfs_msg_readdir_struct
{
	uint32_t msg_type;
	inode_t node;
	uint32_t offset;
} vfs_msg_readdir;

typedef struct vfs_msg_readdir_reply_struct
{
	uint32_t msg_type;
	dirent_t dirent;
} vfs_msg_readdir_reply;

typedef struct vfs_msg_finddir_struct
{
	uint32_t msg_type;
	inode_t node;
	char name[FILE_NAME_LENGTH];
} vfs_msg_finddir;

typedef struct vfs_msg_finddir_reply_struct
{
	uint32_t msg_type;
	inode_t node;
} vfs_msg_finddir_reply;

typedef struct vfs_msg_mount_struct
{
	uint32_t msg_type;
	inode_t mountpoint;
	inode_t root;
} vfs_msg_mount;

typedef struct vfs_msg_touch_struct
{
	uint32_t msg_type;
} vfs_msg_touch;

typedef struct vfs_msg_link_struct
{
	uint32_t msg_type;
	inode_t dir;
	inode_t node;
	unsigned char name[FILE_NAME_LENGTH];
} vfs_msg_link;


typedef uint32_t (*read_ft)(inode_t*, uint32_t, uint32_t, uint8_t*);
typedef uint32_t (*write_ft)(inode_t*, uint32_t, uint32_t, uint8_t*);
typedef void (*open_ft)(inode_t*);
typedef void (*close_ft)(inode_t*);
typedef dirent_t *(*readdir_ft)(inode_t*, uint32_t);
typedef inode_t *(*finddir_ft)(inode_t*, char*);

uint8_t inode_same(inode_t *a, inode_t *b);

dirent_t *readdir(inode_t *node, uint32_t offset);
inode_t *finddir(inode_t *node, char *name)

#endif