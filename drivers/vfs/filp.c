#include <stdint.h>

#include "include/filp.h"

filp_t *new_filp()
{
	filp_t *new = (filp_t *)calloc(sizeof(filp_t));
	return new;
}

void dup_filp(filp_t *filp)
{
	filp->use_counter = filp->use_counter + 1;
}

void put_filp(filp_t *filp)
{
	filp->use_counter = filp->use_counter - 1;
}

void drop_filp(filp_t *filp)
{
	put_filp(filp);
	if(filp->use_counter == 0)
		free(filp);
}