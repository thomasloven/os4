#include <stdint.h>
#include "include/fproc.h"

fproc_t *procs[PROC_NUM];

void init_fproc()
{
	memset(procs,0,sizeof(fproc_t*)*PROC_NUM);
}

fproc_t *new_fproc(uint32_t pid, uint32_t ppid)
{
	if(procs[pid])
		free(procs[pid]);
	procs[pid] = (fproc_t *)malloc(sizeof(fproc_t));
	if(ppid)
		memcopy(procs[pid], procs[ppid], sizeof(fproc_t));
	else
		memset(procs[pid], 0, sizeof(fproc_t));
		
	procs[pid]->pid = pid;
	
	return procs[pid];
}

fproc_t *get_fproc(uint32_t pid)
{
	return procs[pid];
}

void kill_fproc(uint32_t pid)
{
	if(procs[pid])
		free(procs[pid]);
}