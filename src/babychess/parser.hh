#ifndef PARSER_INCLUDED
#define PARSER_INCLUDED

#include "string.hh"
#include "pointer.hh"
#include "text_utils.hh"

class Parser
{
public:
	
	Parser(String text);
	/* Parse TEXT.  TEXT may be empty. 
	 */

	bool End() const
		{
			return index == *text;
		}

	/* The Skip_*() functions advance the index and possibly
	 * return what was read.
	 *
	 * The Next_*() functions read ahead but don't modify the
	 * index. 
	 */

	void Skip_Space()
		{
			index += Skip_Spaces(text() + index);
		}
	String Skip_Word();
	bool Skip_String(String &result);
	/* Read a string enclosed in quotes.  If the result is FALSE:
	 * If RESULT is empty, the next character is not a quote, else
	 * the string is unterminated and RESULT contains the string. 
	 */
	char Next_Char() const
		{
			return text[index];
		}
	char Skip_Char()
		{
			char next= Next_Char();
			if (next == '\0')
				return '\0';
			++ index;
			return next;
		}
	void Skip(int n)
		{
			assert (index + n <= *text);
			index += n;
		}
	
	String Next_Word_Excluding(String exclude);
	String Skip_Word_Excluding(String exclude)
		{
			String result= Next_Word_Excluding(exclude);
			Skip(*result);
			return result; 
		}

	String Skip_Excluding(char exclude);
	String Skip_Number();

private:

	const String text;
	int index;
};

#endif /* ! PARSER_INCLUDED */
