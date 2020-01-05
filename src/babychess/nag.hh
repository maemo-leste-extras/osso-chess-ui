#ifndef NAG_INCLUDED
#define NAG_INCLUDED

/* Numeric annotation glyph -- as defined by the PGN standard.
 */

#include "string.hh"

static const int nag_count= 6;

extern const char *const nag_text[nag_count];
extern const char *const nag_description[nag_count];

int Parse_NAG(String text);
/* Return one more than the index. 
 */

String NAG(int nag);
/* Return the associated NAG. 0 = empty
 */

#endif /* ! NAG_INCLUDED */ 
