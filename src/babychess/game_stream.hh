#ifndef GAME_STREAM_INCLUDED
#define GAME_STREAM_INCLUDED

#include "game.hh"
#include "stream.hh"

bool Read_Stream(Stream &, Game &game);
bool Write_Stream(Stream &, const Game &game); 
/*bool Check_File_Name(String, Game *);*/

#endif /* ! GAME_STREAM_INCLUDED */
