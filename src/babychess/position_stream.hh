#ifndef POSITION_STREAM_INCLUDED
#define POSITION_STREAM_INCLUDED

#include "stream.hh"
#include "position.hh"

bool Read_Stream(Stream &, Position &);
bool Write_Stream(Stream &, const Position &);

#endif /* ! POSITION_STREAM_INCLUDED */
