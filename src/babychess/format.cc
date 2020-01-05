#include "format.hh"

#ifdef FORMAT_OLD_IMPLEMENTATION

#include <stdarg.h>
#include <stdio.h>

String Format(const char *format, ...)
{
	assert (format != NULL);

	va_list vl;

	va_start(vl, format);

	String result(64, 0);
	int sn;
	/* Hopefully VL can be used several times here. */
      loop:
	sn= vsnprintf(result.c(), *result+1, format, vl);
	if (sn < 0)  return "";
	if (sn > *result+1)
	  {
	    Object_Reset(String, &result, sn, 0);
	    goto loop;
	  }

	result.Set_Length(sn);
	
#if GNU_LIBC_IMPLEMENTATION
	String result;
	char *r;
	int size= vasprintf(&r, format, vl);
	{
		void *r2= realloc(r, sizeof (String::Head) + size + 1);
		if (r2 == NULL)
		{
			free (r);
		}
		else
		{
			memmove((char*)r2 + sizeof(String::Head), r2, size + 1);
			String::Head *h= (String::Head *) r2;
			h->guard= 0;
			h->length= size;
			h->ref_count= 1;
			h->place= size;
			Object_Reset(String, &result, r2, 0);
		}
	}
#endif /* GNU */ 
	va_end(vl);

	return result;
}

#endif /* FORMAT_OLD_IMPLEMENTATION */
