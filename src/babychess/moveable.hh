#ifndef MOVEABLE_INCLUDED
#define MOVEABLE_INCLUDED

/* The Moveable() function determines whether an object is moveable.
   If an object is moveable, it can be moved in memory.  The 
   Vector <Type> class moves its objects in memory when resizing. 
 */

#include "inline.hh"
//#include "null.hh"
//#include "assert.hh"

#include <typeinfo> 
#include <stdlib.h>
#include <assert.h>

extern bool (*moveable_hook)(void *, int, const char *name);

Inline
void
Moveable_Set_Hook(bool (*hook)(void *, int, const char *))
{
	assert (hook != NULL);
	assert (moveable_hook == NULL);
	
	moveable_hook= hook; 
}

template <class Type>
Inline
bool
Moveable(const Type *object)
{
	if (object == NULL)
		return true; 

	if (moveable_hook == NULL)
		return true;

	moveable_hook((void *)object, 
		      sizeof (Type),
		      typeid (Type).name()); 

	return true; 
}

#endif /* ! MOVEABLE_INCLUDED */
