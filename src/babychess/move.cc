#include "move.hh"

bool Move::operator==(const Move &move) const
{
	/* do not compare all fields */
	return
		from == move.from &&
		to == move.to &&
//		castling == move.castling &&
		promotion == move.promotion; 
//		ep_square == move.ep_square;
}

void Move::Squares_Touched(int *squares) const
{
	squares[0]= from;
	squares[1]= to;

	if (ep_square != 0)
	{
		squares[2]= ep_square;
		squares[3]= 0;
	}

	else if (castling != Move::no_castling)
	{
		switch (castling)
		{
		case Move::w_O_O:   squares[2]= sq_h1; squares[3]= sq_f1; break;
		case Move::w_O_O_O: squares[2]= sq_a1; squares[3]= sq_d1; break;
		case Move::b_O_O:   squares[2]= sq_h8; squares[3]= sq_f8; break;
		case Move::b_O_O_O: squares[2]= sq_a8; squares[3]= sq_d8; break;

		default: assert (0);
		}
	}
	else
	{
		squares[2]= 0;
		squares[3]= 0;
	}
}

