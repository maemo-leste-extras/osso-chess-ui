#ifndef TRUNCATE_INCLUDED
#define TRUNCATE_INCLUDED

#include "string.hh"

String Truncate(String text, int length);
/* Remove characters from the end of the string so it is at most
 * LENGTH characters long.  If truncated, the resulting string will
 * contain "..." to indicate truncation.  The "..." is counted in
 * LENGTH. 
 */

#endif /* ! TRUNCATE_INCLUDED */

