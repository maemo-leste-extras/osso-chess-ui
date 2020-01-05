#include "nag.hh"

#include "format.hh"

#include <stdlib.h>
#include <string.h>

const char *const nag_text[nag_count]=
{
	"!",
	"?",
	"!!",
	"??",
	"!?",
	"?!",
};

#ifndef gettext_noop
#define gettext_noop(text)  text
#endif
const char *const nag_description[nag_count]=
{
	"good move",
	"poor move",
	"very good move",
	"very poor move",
	"speculative move",
	"questionable move",
};

int Parse_NAG(String text)
{
	for (int i= 0;  i < 6;  ++i)
		if (0 == strcmp(text(), nag_text[i]))
			return i + 1;
	return 0;
}

String NAG(int nag)
{
	assert (nag >= 0);
	
	if (nag == 0)
		return "";

	--nag;
	if (nag >= nag_count)
		return Format("[%d]", nag + 1);
	
	return nag_text[nag];
}

