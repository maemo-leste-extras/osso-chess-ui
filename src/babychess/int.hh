#ifndef INT_INCLUDED
#define INT_INCLUDED

/* The INT module is a wrapper around the ISO C 99 new integer types. 
 *
 * Config macros:
 *           INT_C99:  Have the C99 types. 
 *           INT_HAVE_JD:   printf("%'jd") works
 */

/* Normally the C99 types (found in <stdint.h> are enough).  They are */
/* however not supported on Solaris, and therefore they are defined */ 
/* like this.  None of this is used on systems that have <stdint.h>. */
/* Note that GLib doesn't have the equivalent of intmax_t. */

#include "string.hh"

//#include "int.config.hh"

#include <stdint.h>

#ifdef INT_HAVE_JD
# define INTMAX_SCANF        "jd"
# define INTMAX_PRINTF       "jd"
# define INTMAX_PRINTF_SEP  "'jd"
#else
# define INTMAX_SCANF       "lld"
# define INTMAX_PRINTF      "lld"
# define INTMAX_PRINTF_SEP  "lld"
#endif

#endif /* ! INT_INCLUDED */
