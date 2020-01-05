#ifndef READ_LINE_INCLUDED
#define READ_LINE_INCLUDED

#include "string.hh"
#include "stream.hh"

bool Read_Line(Stream &stream,
	       String &line);
/* Read a line from FILE.  Write the line read into LINE.  Remove
 * terminating newline.  Return FALSE on error or end-of-file.  The
 * resulting LINE may be empty. 
 */

#endif /* ! READ_LINE_INCLUDED */
