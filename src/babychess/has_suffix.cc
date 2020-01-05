#include "has_suffix.hh"

bool
Has_Suffix(String file_name,
	   String suffix)
{
	suffix= String('.') + suffix;
	const char *s1= file_name();
	const char *s2= suffix();
	int l= *suffix;
	const char *f= s1;
	while (f + l < s1 + *file_name)
	{
		f= strstr(f, s2);
		if (f == NULL)
			return false;
		if (f + l == s1 + *file_name)
			return true;
		++ f;
	}
	return false;
}
