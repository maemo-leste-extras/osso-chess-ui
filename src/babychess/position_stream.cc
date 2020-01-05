#include "position_stream.hh"

#include "read_line.hh"
#include "read_fen.hh"
//#include "error.hh"
//#include "check_position.hh"
#include "fen.hh"

bool Read_Stream(Stream     &stream,
		 Position &position)
{
	String line;
	if (! Read_Line(stream, line))
		return false;

	if (! line.Is())
		return false;

	if (! Read_FEN(line,
		       position))
		return false;
	
	return true;
}

bool
Write_Stream(Stream &stream,
	     const Position &position)
{
//	Position tmp_position;
//	tmp_position= position;
//	int n= Check_Position(tmp_position);
//	if (n == nrm_ok)
//	if (tmp_
	if (position.Would_Be_Legal())
	{
		String s= FEN(/*tmp_*/position);
		return (stream.Write_Line(s())  &&
			stream.Write_Char('\n'));
	}
	else
	{
//		Error::Set(Check_Position_Error(n));
		return false;
	}
}
