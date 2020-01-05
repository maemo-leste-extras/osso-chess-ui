#include "constant.hh"

/* Macro set in `macro':
 *
 * -  CONSTANT_NORMAL:  Normal memory layout:  CODE / HEAP / STACK 
 */

#ifdef CONSTANT_NORMAL

#include <malloc.h>
#include <unistd.h>

void *constant_heap= NULL;

void Constant_Make()
{
	assert (constant_heap == NULL);

	struct mallinfo mi= mallinfo();

	constant_heap= (char*)sbrk(0) - mi.arena;

	assert (constant_heap != NULL);
}

#endif /* CONSTANT_NORMAL */
