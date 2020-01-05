#ifndef MOVE_NAME_INCLUDED
#define MOVE_NAME_INCLUDED

/* The name of a chess move. 
 */

#include "piece_type.hh"
#include "square.hh"
#include "int.hh"

class Move_Name
{
public:

	Move_Name() 
		/* Raw */ 
		{
		}
	Move_Name(int new_piece_type, 
		  int new_capture, 
		  int new_from_file,
		  int new_from_rank,
		  int new_to_file, 
		  int new_to_rank,
		  int new_special, 
		  int new_precision, 
		  int new_check,
		  int new_force)
		:  piece_type(new_piece_type), 
		   capture(new_capture), 
		   from_file(new_from_file), 
		   from_rank(new_from_rank),
		   to_file(new_to_file), 
		   to_rank(new_to_rank),
		   special(new_special), 
		   precision(new_precision), 
		   check(new_check),
		   force(new_force)
		{
		}

	unsigned piece_type      :3; 

	unsigned capture         :3; 
	/* Piece type of captured piece + 1 or 0 when not a capture.  
	 */

	unsigned from_file       :3;
	unsigned from_rank       :3;
	unsigned to_file         :3;
	unsigned to_rank         :3;

	unsigned special         :3; 
	/* 0: normal move
	 * 1: en passant
	 * 2: O-O
	 * 3: O-O-O
	 * 4..7: promotion piece type + 3 
	 */

	unsigned precision       :2;
	/* Additional information needed in the short notation for
	 * ambig[u]ous moves. 
	 *
	 *   0:  no additional information needed    Nd7
	 *   1:  show destination file               Nbd7
	 *   2:  show destination rank               R5e7
	 *   3:  show both                           Qa7b8
	 */

	unsigned check           :2;
	/*   0:  no check
	 *   1:  check
	 *   2:  double check
	 *
	 * The move may additionally be mate or stalemate, as
	 * indicated by FORCE. 
	 */

	unsigned force           :1;
	/* Wether the move is mate/stalemate.
	 */
};

#endif /* ! MOVE_NAME_INCLUDED */ 
