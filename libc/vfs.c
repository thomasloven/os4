#include <stdint.h>
#include <vfs.h>
#include <ipc.h>
#include <heap.h>

uint8_t inode_same(inode_t *a, inode_t *b)
{
	if(a->driver_num != b->driver_num)
		return 0;
		
	if(a->inode_num != b->inode_num)
		return 0;
		
	return 1;
}

void chdir(inode_t *node)
{
	CALLOCP(message, vfsm);
	message->m.msg_type = VFSM_CHWD;
	memcopy((uint8_t *)&message->chwd.wd, (uint8_t *)node, sizeof(inode_t));

	send2(VFS_PID,message);
	free(waitmsg(VFS_PID,0));
	
	free(message);
}

void chroot(inode_t *node)
{
	CALLOCP(message, vfsm);
	message->m.msg_type = VFSM_CHRD;
	memcopy((uint8_t *)&message->chrd.rd, (uint8_t *)node, sizeof(inode_t));

	send2(VFS_PID,message);
	free(waitmsg(VFS_PID,0));
	
	free(message);
}

dirent_t dirent;

dirent_t *readdir(inode_t *node, uint32_t offset)
{
	CALLOCP(message, vfsm);
	message->m.msg_type = VFSM_READDIR;
	memcopy((uint8_t *)&message->readdir.node, (uint8_t *)node, sizeof(inode_t));
	message->readdir.num = offset;
	
	send2(VFS_PID, message);
	
	vfsm *reply = waitmsg(VFS_PID,0);
	memcopy((uint8_t *)&dirent, (uint8_t *)&reply->readdir_reply.dirent, sizeof(dirent_t));
	free(message);
	free(reply);
	
	return &dirent;
}

uint32_t open(char *path)
{
	CALLOCP(message, vfsm);
	message->m.msg_type = VFSM_OPEN;
	message->open.path_len = strlen(path)+1;
	message->open.path = path;
	
	send2(VFS_PID, message);
	
	vfsm *reply = waitmsg(VFS_PID,0);
	uint32_t ret = reply->open_reply.filp;
	free(message);
	free(reply);
	
	return ret;
}

stat_t stat;

stat_t *fstat(uint32_t filp)
{
	CALLOCP(message, vfsm);
	message->m.msg_type = VFSM_FSTAT;
	message->fstat.filp = filp;
	
	send2(VFS_PID, message);
	
	vfsm *reply = (vfsm *)waitmsg(VFS_PID,0);
	memcopy((uint8_t *)&stat, (uint8_t *)&reply->fstat_reply.node, sizeof(stat_t));
	free(message);
	free(reply);
	return &stat;
}

uint32_t read(uint32_t fp, uint32_t length, char *buffer)
{
	CALLOCP(message, vfsm);
	message->m.msg_type = VFSM_READ;
	message->read.filp = fp;
	message->read.length = length;
	message->read.buffer = buffer;
	
	send2(VFS_PID, message);
	
	vfsm *reply = (vfsm *)waitmsg(VFS_PID,0);
	uint32_t ret = reply->read_reply.length;
	free(reply);
	return ret;
}


/*
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
*/