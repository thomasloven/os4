#include <stdint.h>
#include <vfs.h>
#include "include/fs.h"
#include "include/inodes.h"
#include "include/tar.h"

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
	
	first_file_node = inode_num;
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
	dir->last->entry.inode_number = new->inode_num;
		
	strcopy(dir->last->entry.name,name);
}

inode_t *new_inode(tar_head *head)
{
	inode_t *node = (inode_t *)calloc(sizeof(inode_t));
	node->inode_num = inode_num++;
	node->driver_num = _syscall_getpid();
	
	fs_last->next = (inode_list *)calloc(sizeof(inode_list));
	fs_last = fs_last->next;
	fs_last->node = node;
	if(head)
	{
		fs_last->tar = head;
	}
	
	return node;
}

inode_t *get_inode(uint32_t inode_num)
{
	inode_list *nodel = fs_first;
	while(nodel->next && nodel->node->inode_num != inode_num)
	{
		nodel = nodel->next;
	}
	if(nodel->node->inode_num != inode_num)
		return 0;
	return nodel->node;
}

tar_head *get_inode_tar(uint32_t inode_num)
{
	inode_list *nodel = fs_first;
	while(nodel->next && nodel->node->inode_num != inode_num)
	{
		nodel = nodel->next;
	}
	if(nodel->node->inode_num != inode_num)
		return 0;
	return nodel->tar;
}
