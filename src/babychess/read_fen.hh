#ifndef READ_FEN_INCLUDED
#define READ_FEN_INCLUDED

/* Decode a FEN string 
 */

#include "string.hh"
#include "position.hh"

bool Read_FEN(String       text,
	      Position    &position);
/* Decode the FEN string TEXT, and set POSITION to the new position.
 * On error, return FALSE and set the error in the ERROR module.  One
 * success, the position is in legal mode. 
 */

#endif /* ! READ_FEN_INCLUDED */
