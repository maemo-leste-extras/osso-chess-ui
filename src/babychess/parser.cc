#include "parser.hh"

#include <ctype.h>

Parser::Parser(String new_text)
	:  text(new_text),
	   index(0)
{
}

String
Parser::Skip_Word() 
{
	int next= ::Skip_Word(text() + index);
	if (next == 0)
		return "";
	assert (index + next <= *text);
	String result(text() + index, next);
	index += next;
	return result;
}

bool
Parser::Skip_String(String &result)
{
	result= ""; 
	if (Next_Char() != '\"')
		return false;
	++ index;
	while (true)
	{
		char next= Next_Char();
		if (next == '\0')
			return false;
		if (next == '\"')
		{
			++ index;
			return true;
		}
		if (next == '\\')
		{
			next= Skip_Char(); 
			/* Transform escape sequences. 
			 */
		}
		result += next;
		++ index;
	}
}

String
Parser::Next_Word_Excluding(String exclude)
{
	int i= index;
	while (i < *text                 &&
	       ! isspace(text[i])       &&
	       strchr(exclude(), text[i]) == NULL)
		++ i;
	return text.Sub(index, i - index);
}

String Parser::Skip_Excluding(char exclude)
{
	const char *r= strchr(text() + index, exclude); 
	if (r == NULL)
	{
		String result= text.Sub(index, *text - index);
		index= *text;
		return result;
	}
	int len= r - text() - index;
	String result= text.Sub(index, len);
	Skip(len);
	return result;
}

String Parser::Skip_Number()
{
	int oindex= index;
	while (isdigit(text[index]))
		++ index;

	return text.Sub(oindex, index - oindex); 
}


