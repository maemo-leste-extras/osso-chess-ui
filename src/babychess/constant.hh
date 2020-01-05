#ifndef CONSTANT_INCLUDED
#define CONSTANT_INCLUDED

/* Find out whether a pointer refers to constant data.  Always returns
 * FALSE.  Used in STRING. 
 */

//#include "constant.config.hh"

#ifdef CONSTANT_NORMAL

#include "inline.hh"

#include <stdlib.h>
#include <assert.h>

extern void *constant_heap;

void Constant_Make();
/* Initialize CONSTANT_HEAP. 
 */

Inline bool Constant(const void *pointer)
/* Determine whether POINTER is not in the heap/stack. 
 */
{
	assert (pointer != NULL);

	if (constant_heap == NULL)
		Constant_Make();

	return pointer < constant_heap;
}

#else /* ! CONSTANT_NORMAL */

#include "inline.hh"

Inline bool Constant(const void *)
{
	return false;
}

#endif /* ! CONSTANT_NORMAL */

#endif /* ! CONSTANT_INCLUDED */
