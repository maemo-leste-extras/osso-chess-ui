#include "printify_char.hh"

#include <ctype.h>

String
Printify_Char(char c)
{
	if (isprint(c))
		return c;
  
	if (c == '\0')
		return "^@";

	if (c >= 1 && c <= 26)
		return (String('^')
			+ (char)('A' + c - 1));

	if (c == 27)
		return "^[";

	if (c == 28)
		return "^\\";

	if (c == 29)
		return "^]";

	if (c == 30)
		return "^^";

	if (c == 31)
		return "^_";

	return '?';
}
