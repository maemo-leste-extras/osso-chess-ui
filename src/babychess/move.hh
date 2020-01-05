#ifndef MOVE_INCLUDED
#define MOVE_INCLUDED

#include "square.hh"
#include "piece.hh"

class Move
/* Information necessary to play the move on the board, and to take it back. 
*/
	:  public Pointed
{
public:

	enum Castling {no_castling= 0, 
		       w_O_O= 4, 
		       w_O_O_O= 5,
		       b_O_O= 6,
		       b_O_O_O= 7}; 

	int from, to;
	/* Squares.  If the move is castling, the squares of the
	 * kings. 
	 */

	Piece piece;
	/* piece that does the move */

	int ep_poss_square;
	/* Where could there have been an "en passant" capture; for instance
	 * e5. 
	 */

	Piece promotion;
	/* empty square:  no promotion
	 * piece:         promotion into... (including color)
	 */

	int ep_square;
	/* non-zero:  this move is an "en passant" capture of this
	 *            square (eg e5)
	 * zero:      the move is not an "en passant" capture
	 */

	Piece captured_piece;
	/* The piece that is captured.  Empty when the move is not a
	 * capture, else the piece that was captured, including the
	 * color.  A pawn also for en passant moves.
	 */

	Castling castling;

	int halfmove_clock;
	/* Halfmove clock of the position before the move. 
	 */
	bool check;
	/* Whether the position was check before the move. 
	 */

	Move() 
		{
		}

	Move(int new_from,
	     int new_to,
	     Piece new_piece,
	     int new_ep_poss_square,
	     Piece new_promotion,
	     int new_ep_square,
	     Piece new_captured_piece, 
	     Castling new_castling)
		: from(new_from),
		  to(new_to), 
		  piece(new_piece),
		  ep_poss_square(new_ep_poss_square), 
		  promotion(new_promotion),
		  ep_square(new_ep_square),
		  captured_piece(new_captured_piece), 
		  castling(new_castling)
		{
		}

	bool operator==(const Move &move) const;
	/* The two moves to compare must be legal in the same position. */

	void Squares_Touched(int *squares) const;
	/* Fills the array of size 4 with squares that are changed by
	   this move.  If less than 4 squares are changed, SQUARES[i]
	   is set to zero. 
	*/

	int Get_Captured_Square() const
		/* The move must be a capture.  Return the square that was captured.  Equal
		   to TO except for "en passant" moves. 
		*/
		{
			return (ep_square
				? ep_square
				: to);
		}

	int Get_Rook_From() const
		/* THIS must be a castling.  Return the FROM square of the rook sub-move. */
		{
			assert (castling != no_castling);
			return Make_Square(File(to) == sq_g
					   ? sq_h
					   : sq_a,
					   Rank(to));
		}
  
	int Get_Rook_To() const
		/* TO equivalent of Get_Rook_From(). */
		{
			assert (castling != no_castling);
			return Make_Square(File(to) == sq_g
					   ? sq_f
					   : sq_d,
					   Rank(to));
		}
};

#endif /* ! MOVE_INCLUDED */
