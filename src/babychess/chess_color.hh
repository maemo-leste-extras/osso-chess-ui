#ifndef CHESS_COLOR_INCLUDED
#define CHESS_COLOR_INCLUDED

/* The color of a chess piece, or the color of a square of a chess
 * board. 
 */

#include "inline.hh"

enum 
{
	cc_b= 0, /* black */
	cc_w= 1  /* white */
};

/* Note:  to have opposite color, use 1^color.  
 */

#endif /* ! CHESS_COLOR_INCLUDED */
