#ifndef FORMAT_INCLUDED
#define FORMAT_INCLUDED

/* Format strings.   sprintf() wrapper. 
 *
 * Usage:         String s= Format("   %s   %d   ",
 *                                 "khjsd",
 *		                   42);
 */

#include "string.hh"

#define FORMAT_OLD_IMPLEMENTATION
/* New one doesn't work. 
 */

#ifdef FORMAT_OLD_IMPLEMENTATION
/* Call a function that uses varargs.  The more elegant solution. 
 */

String
Format(const char *format, ...)
	__attribute__ ((format (printf, 1, 2)));

#else /* ! FORMAT_OLD_IMPLEMENTATION */
/* Use a macro.  Faster. 
 */

#include <stdio.h>

#define FORMAT_LEN   0x80

#define Format(format, args...)	        		\
({							\
        char buf[FORMAT_LEN];                           \
	int s= snprintf(buf,				\
			FORMAT_LEN,			\
			format , ## args);		\
	if (s >= FORMAT_LEN)				\
             buf[FORMAT_LEN-1]= '\0';                   \
 String result(buf);                                    \
 result;                                                \
})


#endif /* ! FORMAT_OLD_IMPLEMENTATION */ 

#endif /* ! FORMAT_INCLUDED */
