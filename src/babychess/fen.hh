#ifndef FEN_INCLUDED
#define FEN_INCLUDED

/* FEN notation for chess positions. 
 *
 * Forsythe-Edwards Notation, as defined by the PGN standard.
 *
 * See <http://www.clark.net/pub/pribut/standard.txt>. 
 */

/* The READ_FEN module contains a function that parses a FEN string. 
 */

#include "position.hh"

String
FEN(const Position &position);
/* Return the FEN description of POSITION. 
 */

#endif /* ! FEN_INCLUDED */
