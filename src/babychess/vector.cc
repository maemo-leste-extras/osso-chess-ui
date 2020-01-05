#include "vector.hh"

#include <string.h>
#include <assert.h>

bool Vector_Base::Set(int n,
		      void (*constructor)(void*, int, int),
		      void (*destructor)(void*, int, int),
		 void (*assert_moveable)(void*, int, int),
		      int sizeof_type)
{
//	Dbg_("%p  Set(), pointer= %p", this, pointer);

	if (n < 0)
		n += size;
	assert (n >= 0);
	if (n < size)
				/* make smaller */
	{
		destructor(pointer, n, size); 

		size= n;
		assert_moveable(pointer, 0, n);


		void *new_pointer= n 
			? realloc(pointer, n * sizeof_type) 
			: (void*)(free(pointer), NULL);
		/* If realloc() fails, keep the current size and
		 * succeed.  
		 */
		if (new_pointer == NULL && n != 0)
			return true;
		pointer= new_pointer;
		place= n;
		return true;
	}
	else if (n > place) /* make bigger than current place */
	{
		int nn= n;
		int plus= place * (100 + 5) / 100; 
		if (plus > nn)
			nn= plus;
		assert_moveable(pointer, 0, size);
		void *new_pointer= size 
			? realloc(pointer, nn * sizeof_type) 
			: malloc(nn * sizeof_type);
		if (new_pointer == NULL)
			return false;

		pointer= new_pointer;
		constructor(pointer, size, n); 

		size= n;
		place= nn;
		return true;
	}
	else   /* make bigger, but have enough place */
	{
		constructor(pointer, size, n); 
		size= n;
		return true;
	}
}

bool 
Vector_Base::Append_Transfer_From(Vector_Base &vector,
				  int i,
				  void (*assert_moveable)(void*, int, int),
				  int sizeof_type)
{
	assert (i >= 0);
	assert (i < vector.size);

	assert_moveable(vector.pointer, i, vector.size);

	void *new_pointer= 
		( size
		  ? realloc (pointer,  (size + vector.size - i) * sizeof_type)
		  : malloc  (          (size + vector.size - i) * sizeof_type));
	if (new_pointer == NULL)
		return false;
	pointer= new_pointer;
	memcpy(((char*)pointer) + size * sizeof_type,
	       ((char*)vector.pointer) + i * sizeof_type,
	       (vector.size - i) * sizeof_type);
	size += vector.size - i;
	place= size;
	vector.size= i;
	new_pointer= (i
		      ? realloc(vector.pointer, i * sizeof_type)
		      : (void*)(free (vector.pointer), NULL));
	if (new_pointer != NULL || i == 0)
	{
		vector.pointer= new_pointer;
		vector.place= i;
	}

	return true; 
}

