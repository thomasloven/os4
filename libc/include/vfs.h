#ifndef VFS_H
#define VFS_H

#include <stdint.h>

#define VFS_PID	0x2

#define VMSG_READ	0x11
#define VMSG_WRITE	0x12
#define VMSG_OPEN	0x13
#define VMSG_CLOSE	0x14
#define VMSG_READDIR	0x15
#define VMSG_FINDDIR	0x16
#define VMSG_MOUNT	0x17
#define VMSG_TOUCH	0x18
#define VMSG_REPLY 0x20

#define VFSM_FORK	0x01
#define VFSM_CHRD	0x02
#define VFSM_CHWD	0x03
#define VFSM_OPEN	0x04
#define VFSM_GETNODE	0x05
#define VFSM_READDIR	0x06
#define VFSM_FSTAT		0x07
#define VFSM_LOOKUP		0x08
#define VFSM_READ		0x09
#define VFSM_READ_INT	0x0A

#define VFSM_REPLY	0x10
#define VFSM_RMOUNT	0x20

#define FILE_NAME_LENGTH 128
#define FILE_PATH_LENGTH 1024

#define VFS_FILE		0x1
#define VFS_DIRECTORY	0x2
#define VFS_CHARDEVICE	0x3
#define VFS_BLOCKDEVICE	0x4
#define VFS_PIPE		0x5
#define VFS_SYMLINK		0x6
#define VFS_MOUNTPOINT	0x8

/*typedef struct vfs_inode_struct
{
	uint32_t driver_num;
	uint32_t inode_num;
	uint32_t flags;
	uint32_t use_counter;
} inode_t;*/



typedef struct dirent_struct
{
	char name[FILE_NAME_LENGTH];
	uint32_t inode_number;
} dirent_t;

typedef struct vfs_msg_struct
{
	uint32_t msg_type;
} vfs_msg;

typedef struct stat
{
	uint32_t flags;
	uint32_t inode_num;
	uint32_t driver_num;
	uint32_t uid;
	uint32_t gid;
	uint32_t atime;
	uint32_t ctime;
	uint32_t mtime;
	uint32_t use_counter;
	uint32_t size;
} stat_t;

typedef stat_t inode_t;


typedef struct
{
	uint32_t msg_type;
	uint32_t parent;
	uint32_t child;
} vfs_msg_fork;

typedef vfs_msg vfs_msg_fork_reply;

typedef struct
{
	uint32_t msg_type;
	inode_t rd;
} vfs_msg_chrd;

typedef vfs_msg vfs_msg_chrd_reply;

typedef struct
{
	uint32_t msg_type;
	inode_t wd;
} vfs_msg_chwd;

typedef vfs_msg vfs_msg_chwd_reply;

typedef struct
{
	uint32_t msg_type;
	uint32_t path_len;
	char *path;
} vfs_msg_open;

typedef struct
{
	uint32_t msg_type;
	uint32_t filp;
} vfs_msg_open_reply;

typedef struct
{
	uint32_t msg_type;
	inode_t node;
} vfs_msg_getnode;

typedef vfs_msg_getnode vfs_msg_getnode_reply;

typedef struct
{
	uint32_t msg_type;
	inode_t node;
	uint32_t num;
} vfs_msg_readdir;

typedef struct
{
	uint32_t msg_type;
	dirent_t dirent;
} vfs_msg_readdir_reply;

typedef struct
{
	uint32_t msg_type;
	uint32_t filp;
} vfs_msg_fstat;

typedef vfs_msg_getnode vfs_msg_fstat_reply;

typedef struct
{
	uint32_t msg_type;
	inode_t root;
	uint32_t path_len;
	char *path;
} vfs_msg_lookup;

typedef struct
{
	uint32_t msg_type;
	inode_t node;
} vfs_msg_lookup_reply;

typedef struct
{
	uint32_t msg_type;
	inode_t mp;
	uint32_t processed;
} vfs_msg_lookup_mount;

typedef struct
{
	uint32_t msg_type;
	uint32_t filp;
	uint32_t length;
	char *buffer;
} vfs_msg_read;

typedef struct
{
	uint32_t msg_type;
	uint32_t length;
} vfs_msg_read_reply;

typedef struct
{
	uint32_t msg_type;
	inode_t node;
	uint32_t offset;
	uint32_t length;
} vfs_msg_read_int;

typedef struct
{
	uint32_t msg_type;
	uint32_t length;
	char *buffer;
} vfs_msg_read_int_reply;


typedef uint32_t (*read_ft)(inode_t*, uint32_t, uint32_t, uint8_t*);
typedef uint32_t (*write_ft)(inode_t*, uint32_t, uint32_t, uint8_t*);
typedef void (*open_ft)(inode_t*);
typedef void (*close_ft)(inode_t*);
typedef dirent_t *(*readdir_ft)(inode_t*, uint32_t);
typedef inode_t *(*finddir_ft)(inode_t*, char*);

uint8_t inode_same(inode_t *a, inode_t *b);

dirent_t *readdir(inode_t *node, uint32_t offset);
inode_t *finddir(inode_t *node, char *name);

uint32_t open(char *path);
stat_t *fstat(uint32_t filp);
uint32_t read(uint32_t fp, uint32_t length, char *buffer);

void chdir(inode_t *node);
void chroot(inode_t *node);


typedef union
{
	vfs_msg m;
	vfs_msg_fork fork;
	vfs_msg_fork_reply fork_reply;
	vfs_msg_chrd chrd;
	vfs_msg_chrd_reply chrd_reply;
	vfs_msg_chwd chwd;
	vfs_msg_chwd_reply chwd_reply;
	vfs_msg_open open;
	vfs_msg_open_reply open_reply;
	vfs_msg_getnode getnode;
	vfs_msg_getnode_reply getnode_reply;
	vfs_msg_readdir readdir;
	vfs_msg_readdir_reply readdir_reply;
	vfs_msg_fstat fstat;
	vfs_msg_fstat_reply fstat_reply;
	vfs_msg_lookup lookup;
	vfs_msg_lookup_reply lookup_reply;
	vfs_msg_lookup_mount lookup_mount;
	vfs_msg_read read;
	vfs_msg_read_reply read_reply;
	vfs_msg_read_int read_int;
	vfs_msg_read_int_reply read_int_reply;
} vfsm;

#define ALLOC_VFSM(NAME, TYPE) vfsm *NAME = (vfsm *)malloc(sizeof(TYPE))
#define CALLOC_VFSM(NAME, TYPE) vfsm *NAME = (vfsm *)calloc(sizeof(TYPE))

#endif