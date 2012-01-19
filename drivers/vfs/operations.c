#include <stdint.h>
#include <vfs.h>
#include <ipc.h>
#include <heap.h>
#include "include/operations.h"
#include "include/vnode.h"

void vfs_fork(vfsm *message, uint32_t sender)
{
	if(message->m.msg_type != VFSM_FORK)
		return;
		
	fproc_t *new = new_fproc(message->fork.child, message->fork.parent);
	if(new->wd)
		dup_vnode(new->wd);
	if(new->rd)
		dup_vnode(new->rd);
	
	message->m.msg_type = VFSM_FORK | VFSM_REPLY;
	send2(sender,message);

}

void vfs_chrd(vfsm *message, uint32_t sender)
{
	if(message->m.msg_type != VFSM_CHRD)
		return;
	
	fproc_t *fproc = get_fproc(sender);
	vnode_t *old = fproc->rd;
	fproc->rd = get_vnode(message->chrd.rd.driver_num, message->chrd.rd.inode_num);
	if(old != fproc->rd)
	{
		if(old) drop_vnode(old);
		dup_vnode(fproc->rd);
	}
	
	message->m.msg_type = VFSM_CHRD | VFSM_REPLY;
	send2(sender,message);
}

void vfs_chwd(vfsm *message, uint32_t sender)
{
	if(message->m.msg_type != VFSM_CHWD)
		return;
	
	fproc_t *fproc = get_fproc(sender);
	vnode_t *old = fproc->wd;
	fproc->wd = get_vnode(message->chwd.wd.driver_num, message->chwd.wd.inode_num);
	if(old != fproc->wd)
	{
		if(old) drop_vnode(old);
		dup_vnode(fproc->wd);
	}
	
	message->m.msg_type = VFSM_CHWD | VFSM_REPLY;
	send2(sender,message);
}

void vfs_open(vfsm *message, uint32_t sender)
{
	if(message->m.msg_type != VFSM_OPEN)
		return;
	
	char *path = (char *)malloc(message->open.path_len);
	_syscall_pull(path, message->open.path, sender, message->open.path_len);
	
	CALLOCP(driver_msg, vfsm);
	
	driver_msg->m.msg_type = VFSM_LOOKUP;
	
	fproc_t *fproc = get_fproc(sender);
	_syscall_printf(path);
	_syscall_printf(&path[1]);
	if(path[0] == '/')
	{
		memcopy((uint8_t *)&driver_msg->lookup.root, (uint8_t *)fproc->rd, sizeof(vnode_t));
		driver_msg->lookup.path = &path[1];
		driver_msg->lookup.path_len = message->open.path_len-1;
	}
	else
	{
		memcopy((uint8_t *)&driver_msg->lookup.root, (uint8_t *)fproc->wd, sizeof(vnode_t));
		driver_msg->lookup.path = &path[0];
		driver_msg->lookup.path_len = message->open.path_len;
	}
	send2(driver_msg->lookup.root.driver_num, driver_msg);
	_syscall_printf(path);

	vfsm *reply = (vfsm *)waitmsg(driver_msg->lookup.root.driver_num,0);
	while(reply->m.msg_type != (VFSM_LOOKUP | VFSM_REPLY))
	{
		_syscall_printf("Error!");
		for(;;);
	}
	free(driver_msg);
	
	vnode_t *vnode = get_vnode(reply->lookup_reply.node.driver_num, reply->lookup_reply.node.inode_num);
	
	uint32_t filp = 0;
	while(fproc->filp[filp]) filp++;
	
	fproc->filp[filp] = new_filp();
	

	reply->open_reply.msg_type = VFSM_OPEN | VFSM_REPLY;
	reply->open_reply.filp = filp;
	send2(sender,reply);
	free(reply);
	
}

void vfs_readdir(vfsm *message, uint32_t sender)
{
	if(message->m.msg_type != VFSM_READDIR)
		return;

	send2(message->readdir.node.driver_num,message);
	vfsm *reply = (vfsm *)waitmsg(message->readdir.node.driver_num,0);
	send2(sender, reply);
	free(reply);
}