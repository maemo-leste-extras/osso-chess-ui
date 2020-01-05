#ifndef OBJECT_INCLUDED
#define OBJECT_INCLUDED

#include <stdlib.h>
#include <new>

#define Object_Reset(Type, pointer, args...) \
( 					     \
	(pointer)->~Type(),		     \
	new (pointer) Type(args)	     \
)

#endif /* ! OBJECT_INCLUDED */
