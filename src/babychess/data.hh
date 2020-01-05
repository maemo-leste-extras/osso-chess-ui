#ifndef DATA_INCLUDED
#define DATA_INCLUDED

/* Allocate an object dynamically.  Equivalent of standard C++
 * new()/delete().  See etc/DATA
 */

#include "object.hh"
#include "inline.hh"


/* Common definitions */
#define Data_Alloc(Type, args...)				\
({								\
	Type *data_alloc_object;				\
	try							\
	{							\
		data_alloc_object= new Type(args);		\
		assert (data_alloc_object != NULL); 		\
	}							\
	catch (...)						\
	{							\
		data_alloc_object= NULL;			\
	}							\
	data_alloc_object;					\
})

#define Data_Free(object)          \
((void)((((object) != NULL) ? (({delete object;8;})==0) : 0)))


#endif /* ! DATA_INCLUDED */
