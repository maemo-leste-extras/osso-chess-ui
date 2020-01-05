#include "truncate.hh"

String Truncate(String text, int length)
{
	assert (length >= 4);

	if (*text <= length)
		return text;

	return text.Sub(0, length - 3) + "...";
}

