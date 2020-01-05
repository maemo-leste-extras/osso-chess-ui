#include "text_utils.hh"

#include <ctype.h>

bool Compare_Words(const char *s0, const char *s1)
{
	assert(s0 && s1);

	while (!isspace(*s0) && *s0 && *s0==*s1)
	{++s0; ++s1; }

	return (isspace(*s0) || !*s0) && (isspace(*s1) || !*s1);
}

int Get_Next_Word(const char *c)
{
	assert (c != NULL);
	const char *d= c;
	while (*d && !isspace(*d))
		++d;
	while (isspace(*d))
		++d;
	return d-c;
}

int Skip_Spaces(const char *c)
{
	assert (c != NULL);
	const char *d= c;
	while (isspace(*d))
		++ d;
	return d - c;
}

int Skip_Word(const char *c)
{
	assert (c != NULL);
	const char *d= c;
	while (*d != '\0' && ! isspace(*d))
		++ d;
	return d - c;
}

bool Contains_String(const char *c0, const char *c1)
	/* if c0 begins with c1. Both not NULL
   */
{
	assert (c0 != NULL && c1 != NULL);

	while (*c1 != '\0' && *c0 == *c1)
	{++c0; ++c1; }

	return *c1 == '\0';
}
