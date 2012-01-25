#include <stdint.h>
#include <vfs.h>
#include <ipc.h>
#include <heap.h>
#include "include/fs.h"
#include "include/operations.h"
#include "include/inodes.h"

void tarfs_getnode(vfsm *message, uint32_t sender, tar_head *files)
{
	if(message->m.msg_type != VFSM_GETNODE)
		return;
	
	if(inode_same(&message->getnode.node,root_node))
		memcopy((uint8_t *)&message->getnode.node, (uint8_t *)root_node, sizeof(inode_t));
	else
	{
		uint32_t *data = tar_num(files, message->getnode.node.inode_num-2);
		inode_t *node = get_inode(message->getnode.node.inode_num);
		if(node)
		{
			memcopy((uint8_t *)&message->getnode.node, (uint8_t *)node, sizeof(inode_t));
		} else {
			message->getnode.node.inode_num = 0;
			message->getnode.node.driver_num = 0;
		}
	}
	message->getnode_reply.msg_type = VFSM_GETNODE | VFSM_REPLY;
	send2(sender,&message->getnode_reply);
}


void tarfs_readdir(vfsm *message, uint32_t sender)
{
	if(message->m.msg_type != VFSM_READDIR)
		return;

	dirent_t *dent = fs_readdir(&message->readdir.node, message->readdir.num);
	CALLOCP(reply, vfs_msg_readdir_reply);
	memcopy((uint8_t *)&reply->dirent, (uint8_t *)dent, sizeof(dirent_t));
	send2(sender, reply);
	free(reply);
}

void tarfs_lookup(vfsm *message, uint32_t sender)
{
	if(message->m.msg_type != VFSM_LOOKUP)
		return;
	
	char *path = (char *)malloc(message->lookup.path_len);
	_syscall_pull(path, message->lookup.path, sender, message->lookup.path_len);
	_syscall_printf(path,0);
	uint32_t inode_num = fs_finddir(&message->lookup.root, path);
	memcopy((uint8_t *)&message->lookup_reply.node, (uint8_t *)get_inode(inode_num), sizeof(inode_t));
	message->lookup_reply.msg_type = VFSM_LOOKUP | VFSM_REPLY;
	send2(sender, &message->lookup_reply);
}

uint32_t tarfs_read(vfsm *message, uint32_t sender)
{
		_syscall_printf("Read call %x", message->m.msg_type);
	if(message->m.msg_type != VFSM_READ_INT)
		return;
		
	tar_head *file = get_inode_tar(message->read_int.node.inode_num);
	uint32_t offset = message->read_int.offset;
	int32_t length = message->read_int.length;
	if((offset + length) > tar_size(file->size))
	{
		length = tar_size(file->size) - offset;
	}
	if(length < 0) length = 0;
	
	char *data = (char *)((uint32_t)file + sizeof(tar_head) + offset);
	
	char *buffer = (char *)calloc(length);
	
	memcopy(buffer, data, length);
	
	message->read_int_reply.msg_type = VFSM_READ_INT | VFSM_REPLY;
	message->read_int_reply.buffer = buffer;
	message->read_int_reply.length = length;
	
	send2(sender, message);
	
	_syscall_printf("Read call2",0);
}

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



uint32_t fs_finddir(inode_t *node, char *name)
{
	directory *dir;
	if(inode_same(node,root_node))
	{
		dir = &root_dir;
	}
	dirent_list *d = dir->first;
	while(d && strcmp(&d->entry.name, name))
	{
		d = d->next;
	}
	if(strcmp(&d->entry.name, name))
		return 0;
	return d->entry.inode_number;
}




