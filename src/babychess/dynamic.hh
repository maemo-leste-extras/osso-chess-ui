#ifndef DYNAMIC_INCLUDED
#define DYNAMIC_INCLUDED

#include "data.hh"
//#include "this.hh"

#include <assert.h>

template <class Type> 
class Dynamic
{
public:

	Dynamic() 
		:  pointer(NULL)  
		{
		}

	Dynamic (const Dynamic <Type> &dynamic)
		{
			if (dynamic.Is())
			{
				pointer= Data_Alloc(Type, dynamic());
			}
			else
				pointer= NULL;
		}

	~Dynamic() 
		{
			Data_Free(pointer); 
		}

	const Dynamic <Type> &operator = (const Dynamic <Type> &object)
		/* OBJECT should not be of a type derived from Type. 
		 */
		{
			Data_Free(pointer);
			if (object.Is())
			{
				pointer= Data_Alloc(Type, object());
			}
			else
			{
				pointer= NULL;
			}
			return *this;
		}

	template <class U>
	void Transfer_To(Dynamic <U> &u)
		{
			Type *p= Detach();
			u.Attach(p);
		}
	template <class U>
	void operator >> (Dynamic <U> &u)
		{
			Transfer_To(u); 
		}

	template <class U>
	void operator << (Dynamic <U> &u)
		{
			u.Transfer_To(*this); 
		}

	bool Is() const 
		{
			return pointer != NULL; 
		}
	Type &operator () () 
		{
			return *pointer; 
		}
	Type const& operator() () const 
		{
			return *pointer; 
		}

	Type *operator* () 
		{
			return pointer; 
		}

	Type const *operator* () const 
		{
			return pointer; 
		}

	template <class U>
	bool Attach(U *u) 
		/* U is a sub-type of Type.  Return whether U was not
		 * NULL.  
		 */
		{
			if (Is())
				Free();
			pointer= u; 
			return u != NULL;
		}

	Type *Detach()
		{
			Type *p= pointer;
			pointer= NULL;
			return p;
		}

	bool Alloc() 
		/* Must not be.  Make new object using the default constructor.
		   Return value: FALSE on error, else TRUE. 
		*/
		{
			assert (! Is());
			pointer= Data_Alloc(Type);
			return pointer != NULL;
		}

	bool Alloc(const Type &t)
		/* Like Alloc() but call copy constructor. 
		 */
		{
			assert (! Is());
			pointer= Data_Alloc(Type, t);
			return pointer != NULL;
		}

	template <class U>
	bool Alloc(U &u)
		{
			assert (! Is());
			pointer= Data_Alloc(Type, u);
			return pointer != NULL;
		}

	template <class U, class V>
	bool Alloc(const U &u, const V &v)
		{
			assert (! Is());
			pointer= Data_Alloc(Type, u, v);
			return pointer != NULL;
		}

	void Free() 
		/* Need not be. */
		{
			Data_Free(pointer);
			pointer= NULL;
		}
	const Type *Get() const
		{
			return pointer;
		}

	operator bool () const
		{
			return pointer != NULL; 
		}
	Type *operator -> () const
		{
			return pointer;
		}
private:

	Type *pointer;
};

#endif /* ! DYNAMIC_INCLUDED */
