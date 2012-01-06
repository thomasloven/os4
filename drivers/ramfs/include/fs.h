#pragma once

typedef struct fs_inode_list inode_list;
typedef struct fs_dirent_list dirent_list;
#include <stdint.h>
#include <vfs.h>

typedef struct fs_inode_list
{
	inode_list *next;
	inode_t *node;
} inode_list;

typedef struct fs_dirent_list
{
	dirent_list *next;
	dirent_t entry;
} dirent_list;

typedef struct fs_dir
{
	dirent_list *first;
	dirent_list *last;
} directory;

inode_list *fs_first;
inode_list *fs_last;
uint32_t inode_num;

inode_t *root_node;

directory root_dir;

