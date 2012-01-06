#include <stdint.h>
#include <vfs.h>
#include "include/fs.h"
#include "include/inodes.h"

void init_inodes()
{
	inode_num = 1;
	fs_first = fs_last = (inode_list *)calloc(sizeof(inode_list));
	root_node = fs_first->node = new_inode(0);
	root_node->flags = VFS_DIRECTORY;
	root_node->use_counter = 1;
	
	root_dir.last = root_dir.first = (dirent_list *)calloc(sizeof(dirent_list));
	add_node(root_node, root_node ,".");
	free(root_dir.first);
	root_dir.first = root_dir.last;
	add_node(root_node, root_node ,"..");
}

void add_node(inode_t *node, inode_t *new, char *name)
{
	directory *dir;
	if(node == root_node)
	{
		dir = &root_dir;
	}
	
	dir->last->next = (dirent_list *)calloc(sizeof(dirent_list));
	dir->last = dir->last->next;
	dir->last->entry.inode_number = new->inode_number;
	strcopy(dir->last->entry.name,name);
}

inode_t *new_inode(inode_t *innode)
{
	inode_t *node = (inode_t *)calloc(sizeof(inode_t));
	if(innode)
	{
		memcopy(node, innode, sizeof(inode_t));
	} else {
		node->inode_number = inode_num++;
		node->driver_num = _syscall_getpid();
	}
	return node;
}
