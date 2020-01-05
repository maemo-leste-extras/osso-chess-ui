#ifndef PIECE_INCLUDED
#define PIECE_INCLUDED

/* Chess piece. */

#include "chess_color.hh"
#include "piece_type.hh"
#include "int.hh"

#include <assert.h>

class Piece
/* A chess piece of a specific color, or an "empty square", or a
 * square outside the chess board. 
 */
{
public:

	Piece() 
		{
		}

	Piece(int chess_color, 
	      int piece_type)
		/* Not empty and not outside. */
		{
			assert (chess_color == cc_w  || chess_color == cc_b);
			assert (piece_type >= 0 && piece_type < 6);

			value= (8 << chess_color) | piece_type; 
		}

	Piece(bool outside)
		/* OUTSIDE=
		 *   FALSE:  empty square
		 *   TRUE :  outside the chess board
		 */
		{
			if (outside)
				value= 64;
			else /* empty */
				value= 32;
		}

	void Set_Empty() {
		value = 32;
	}

	void operator = (Piece piece)
		{
			value = piece.value;
		}

	bool operator == (Piece piece) const
		{
			return value == piece.value;
		}

	bool Is() const 
		/* Whether THIS is a piece.  FALSE when an empty square or a
		 * square outside the chess board. 
		 */
		{
			return value & 24;
		}

	operator bool () const
		{
			return Is(); 
		}

	int Get_Type() const
		/* Must not be empty or outside.
		 */
		{
			assert (Is());
			assert ( (value & 7) <6 );
			return value & 7;
		}

	bool Is_Type(int piece_type) const 
		{
			return Is() && Get_Type() == piece_type;
		}

	bool Is_Empty() const 
		/* Whether THIS is an empty piece. */
		{
			return value == 32;
		}

	int Get_Color() const
		/* must be an actual piece */
		{
			assert(Is());
			return value >> 4;
		}

	bool Is_Piece_Of_Color(int color) const
		{
			assert (color == cc_w || color == cc_b);
			return value & (8 << color);
		}

	bool Is_Piece_Of_Type(int piece_type) const
		{
			assert (piece_type >= 0 && piece_type < 6);
			return (value & ~24) == piece_type; 
		}

	bool Can_Color_Move_To(int color)
		/* Whether a piece of COLOR can move to square with
		 * this piece;  empty squares and out-of-board squares
		 * are recognized. 
		 */
		{
			assert (color == cc_w || color == cc_b);
			return value & (32 | (8 << (color ^ 1))); 
		}

	bool operator != (Piece piece) const
		{
			return value != piece.value;
		}
	uint8_t Get_Value() const {return value; }

	void Invert()
		{
			assert (Is());
			value ^= 24;
		}

private:
	
	uint8_t value;
	/* Values:
	 *
	 *    8..13:    white pieces
	 *    16..21:   black pieces
	 *    32:       empty square  (a space character coincidentally)
	 *    64:       outside
	 *
	 * Bits:
	 *
	 *    0..2:     piece type    > zero if not a piece
	 *    3:        white         \
	 *    4:        black         |
	 *    5:        empty         | exactly one of those four is set
	 *    6:        outside       /
	 */
};

#define  pi_wP      Piece(cc_w, pt_P)
#define  pi_wR      Piece(cc_w, pt_R)
#define  pi_wK      Piece(cc_w, pt_K)
#define  pi_bP      Piece(cc_b, pt_P)
#define  pi_bR      Piece(cc_b, pt_R)
#define  pi_bK      Piece(cc_b, pt_K)
#define  pi_empty   Piece(false)
#define  pi_outside Piece(true) 

#endif /* ! PIECE_INCLUDED */
