#ifndef POINTER_INCLUDED
#define POINTER_INCLUDED

/* Pointer <TYPE> points to an object of type TYPE.  TYPE must have
 * Pointed as a base class.  In debug mode, each such object is
 * reference-counted.  
 *
 * The Moveable() function of the MOVEABLE module checks whether an
 * object can be moved in memory.  `Pointed' objects can be moved if
 * their reference count is zero. 
 *
 * TYPE can contain qualifiers such as `const'.  
 */

#include "moveable.hh"
#include "inline.hh"

#include <stdlib.h>
#include <assert.h>

/* Reference counting enabled. 
 */
#include <typeinfo>

class Pointed 
{
public:

	Pointed()
		:  ref_count(0)
		{
		}
	Pointed(const Pointed &)
		:  ref_count(0)
		{
		}
	virtual ~Pointed();

	void operator = (const Pointed &)
		{
		}

	void Pointed_Do() const
		{
		}

	void Inc(const char *new_name,
		 const void *new_pointer) const
		{
			((Pointed *)this)->name= new_name; 
			((Pointed *)this)->pointer= new_pointer; 
			++ ((Pointed *)this)->ref_count;
		}
	void Dec() const
		{
			assert (ref_count > 0);
			-- ((Pointed *)this)->ref_count; 
		}


private:

	int ref_count; 
	const char *name;
	const void *pointer;
};

void
Pointer_Set(const void *,   /* pointer to Type */ 
	    int,
	    const char *name,
	    const Pointed *thisptr);  /* pointer to pointed */ 
void     
Pointer_Remove(const void *,
	       int,
	       const char *name);

extern bool pointer_check;

/* The size taken by an object of type TYPE, whithout the virtual base
 * classes, as they're not counted when TYPE itself is a base class
 */
template <class Type>
inline int Pointer_Size()
{
	class Alpha
		:  virtual public Type
	{
	};
	
	pointer_check= true;
	Alpha *a= new Alpha;
	assert (a);
	Type *t= a;

	void *real_end= (void*)(a + 1);
	void *type_begin= (void *)  t;

	delete a; 

	pointer_check= false;

	return (int)real_end - (int)type_begin;
}

/* Common definition. 
 */

template <class Type> 
class Pointer
{
private:

	static void Pointer_Do_Set(const Type *object)
		{
			if (pointer_check)
				return;
			if (object == NULL)
				return;
			object->Inc(typeid(Type).name(),
					  object); 
			
			Pointer_Set(object,
				    Pointer_Size<Type>(),
				    typeid (Type).name(),
				    (Pointed *)object); 
		}

	static void Pointer_Do_Remove(const Type *object)
		{
			if (pointer_check)
				return;
			if (object == NULL)
				return;
			object->Dec();
			Pointer_Remove(object,
				       Pointer_Size<Type>(),
				       typeid (Type).name());
		}

public:

	Pointer()
		:  object (NULL)
		{
		}
	Pointer(const Pointer <Type> &pointer)
		:  object (pointer.object)
		{
			Pointer_Do_Set(object); 
		}
	Pointer(Type &new_object)
		{
			object= & new_object;
			Pointer_Do_Set(object); 
		}
	Pointer(Type *new_object)
		{
			object= new_object; 
			Pointer_Do_Set(object); 
		}
	~Pointer()
		{
			/* The function Pointed_Do() is called to make
			 * sure that Type is derived from Pointed. 
			 */
			if (object != NULL)
				object->Pointed_Do(); 
			Pointer_Do_Remove(object);
		}
	
	bool Is() const
		{
			return object != NULL; 
		}

	Type &operator () () const
		/* The return value is not a const reference.  The
		 * object that is pointed to is not owned by this
		 * object.  For pointers to constant objects, 
		 * Pointer <const Type> is used. 
		 */
		{
			assert (object != NULL); 
			return *object; 
		}

	
	Type *Get_Pointer() const
		{
			return object; 
		}
	void operator = (Type *new_object)
		{
			Pointer_Do_Remove(object);
			object= new_object;
			Pointer_Do_Set(object); 
		}
	void operator = (const Pointer <Type> &other_object)
		{
			Pointer_Do_Remove(object);
			object= other_object.object; 
			Pointer_Do_Set(object); 
		}

	void operator = (Type &new_object)
		{
			Pointer_Do_Remove(object);
			object= & new_object;
			Pointer_Do_Set(object); 
		}
	
	template <class Type_2> void operator = (const Pointer <Type_2> &object_2)
		{
			Pointer_Do_Remove(object);
			object= &object_2(); 
			Pointer_Do_Set(object); 
		}

	bool operator == (const Type *other_object) const
		{
			return object == other_object; 
		}
	bool operator != (Type *other_object) const
		{
			return object != other_object; 
		}
	bool operator == (const Pointer <Type> &other_object) const
		{
			return object == other_object.object; 
		}
	Type *operator -> () const
		{
			return object;
		}
	operator bool () const
		{
			return object != NULL; 
		}

private:

	Type *object; 
}; 

template <class T>
Inline
bool
operator == (const T *t,
	     const Pointer <T> &pointer)
{
	return t == pointer.Get_Pointer();
}

#endif /* ! POINTER_INCLUDED */
