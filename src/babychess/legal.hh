#ifndef LEGAL_INCLUDED
#define LEGAL_INCLUDED

/* Generate legal moves. 
 */

/* Definition:  Pseudo-legal moves are the legal moves plus those that
 *  put (or keep) the own king in check.   
 */

#include "position.hh"
#include "vector.hh"

class Legal
{
public:

	enum {max_leg= 262};
	/* Maximum possible number of moves in a position. 
	 */

	/* flags */ 
	enum {gn_promotion= (1 << 0),};

	static void Generate(Position &position, 
			     Vector <Move> &moves,
			     bool strict_legal,
			     bool just_check);
	/* Generate legal moves in POSITION.
	 *
	 * Append moves to the array MOVES.
	 *
	 * If STRICT_LEGAL is TRUE, only legal moves are generated.  If it is
	 * false, "pseudo-legal" moves are also generated.
	 *
	 * If JUST_CHECK is TRUE, just checks *whether* there are
	 * (pseudo-)legal moves, but don't generate any.  MOVES is still used
	 * in this case.  There is at least one (legal) move if the size of
	 * MOVES is non-zero. 
	 */
	
	static int flags; 
	/* Set by Generate_Legal().  Contains the gn_* flags. 
	 */

private:

	static bool Pawn(Piece from_piece);
	static bool Add_Move(int to, 
				    Piece captured_piece,
				    Piece promotion, 
				    int ep_capture, 
				    Move::Castling castling= Move::no_castling);
	/* Return value:  whether generation should terminate. 
	 */
};

#endif /* ! LEGAL_INCLUDED */
