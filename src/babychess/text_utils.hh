#ifndef TEXT_UTILS_INCLUDED
#define TEXT_UTILS_INCLUDED

#include "string.hh"
#include "inline.hh"

Inline
bool
Is_File_Name_Char(char c)
{
	return c > 32 && c < 127 && c != ';';
}

bool
Compare_Words(const char *s0, const char *s1);
/* both not NULL. compares up to the next whitespace.
 */

int
Get_Next_Word(const char *c);
/* Beginning of next word or ending '\0'. 
   C must not be NULL.
 */

int
Skip_Spaces(const char *c);
/* Next non-space character.
   C must not be NULL.
*/

int
Skip_Word(const char *);

bool
Contains_String(const char *c0, const char *c1);
/* if c0 begins with c1. Both not NULL.
 */


#endif /* ! TEXT_UTILS_INCLUDED */
