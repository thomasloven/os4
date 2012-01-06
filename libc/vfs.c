#include <stdint.h>
#include <vfs.h>
#include <ipc.h>

uint8_t inode_same(inode_t *a, inode_t *b)
{
	if(a->driver_num != b->driver_num)
		return 0;
		
	if(a->inode_number != b->inode_number)
		return 0;
		
	return 1;
}

uint32_t read(inode_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
	return 0;
}

uint32_t write(inode_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
	return 0;
}

void open(inode_t *node)
{
	
}

void close(inode_t *node)
{
	
}

dirent_t dirent;

dirent_t *readdir(inode_t *node, uint32_t offset)
{
	vfs_msg_readdir *msg = (vfs_msg_readdir *)malloc(sizeof(vfs_msg_readdir));
	msg->msg_type = VMSG_READDIR;
	memcopy(&msg->node, node, sizeof(inode_t));
	msg->offset = offset;
	
	send2(node->driver_num, msg);
	free(msg);
	msg = waitmsg(node->driver_num,0);
	
	vfs_msg_readdir_reply *reply = (vfs_msg_readdir_reply *)msg;
	
	memcopy(&dirent, &reply->dirent, sizeof(dirent_t));
	free(msg);
	
	return &dirent;
}

inode_t inode;

inode_t *finddir(inode_t *node, char *name)
{
	vfs_msg_finddir *msg = (vfs_msg_finddir *)malloc(sizeof(vfs_msg_finddir));
	msg->msg_type = VMSG_FINDDIR;
	memcopy(&msg->node, node, sizeof(inode_t));
	strcopy(&msg->name, name);
	
	send2(node->driver_num, msg);
	free(msg);
	msg = waitmsg(node->driver_num,0);
	
	vfs_msg_finddir_reply *reply = (vfs_msg_finddir_reply *)msg;
	
	memcopy(&inode, &reply->node, sizeof(dirent_t));
	free(msg);
	return &inode;
}