#include "notation_defaults.hh"

bool Read_Defaults(Stream &stream, 
		 Notation &notation)
{
	(void)stream;

	char *tmp[] = {
		"e.p.",
		"O-O",
		"O-O-O",
		"P",
		"N",
		"B",
		"R",
		"Q",
		"K",
		"x",
		"=",
		"w",
		"b",
		};
	
	int i= 0;
	notation.options = 349;

	for (; i < Notation::s_count;  ++i)
	{
		notation[i]= tmp[i];
	}
	for (; i < Notation::s_count;  ++i)
		notation[i]= "";

	return true;
}
