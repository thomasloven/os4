#include <stdint.h>
#include <vfs.h>
#include "include/fs.h"
#include "include/operations.h"

dirent_t *fs_readdir(inode_t *node, uint32_t num)
{
	directory *dir;
	if(inode_same(node,root_node))
	{
		dir = &root_dir;
	}
	dirent_list *d = dir->first;
	while(num)
	{
		if(d->next)
		{
			d = d->next;
			num--;
		}
		else
		{
			dirent.name[0] = '\0';
			dirent.inode_number = 0;
			return &dirent;
		}
	}
	dirent.inode_number = d->entry.inode_number;
	strcopy(&dirent.name, &d->entry.name);
	return &dirent;
}


