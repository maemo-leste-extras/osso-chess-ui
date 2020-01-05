#include "game_stream.hh"

#include "pgn.hh"
#include "has_suffix.hh"
//#include "error.hh"

#include <libintl.h>

bool Read_Stream(Stream &stream, 
		 Game &game) 
{
	String read_ahead;
	if (! PGN::Parse(stream, game, read_ahead))
		return false;

	Game second_game;
	if (PGN::Parse(stream, second_game, read_ahead))
	{
		//Error::Set(gettext("Archive contains more than one game"));
		return false;
	}
	return true;
}

bool Write_Stream(Stream &stream, 
		  const Game &game)
{
	return PGN::Write(stream, game);
}

/*
bool Check_File_Name(String file_name, Game *)
{
	return true;
}
*/
