#include "fen.hh"

//#include "dbg.hh"
#include "format.hh"

String
FEN(const Position &position)
{
	//Dbg_Function();

	String result;

	/* First Field:  piece placement */

	for (int rank= sq_8; rank >= sq_1; -- rank)
	{
		if (rank != sq_8)
			result += '/';

		int empty_count= 0;

		for (int file= sq_a; file <= sq_h; ++ file)
		{
			Piece pi= position/*.Get_Piece*/[Make_Square(file, rank)];
	    
			if (pi.Is())
			{
				if (empty_count != 0)
				{
					result += '0' + empty_count;
					empty_count= 0;
				}

				const char *const chars= "PNBRQK";
				if (pi.Get_Color() == cc_w)
					result += chars[pi.Get_Type()];
				else
					result += 'a' - 'A' + chars[pi.Get_Type()];
			}
			else
			{
				++ empty_count;
			}
		}

		if (empty_count != 0)
			result += '0' + empty_count;
	}

	result += ' ';

	/* Second Field:  active color */

	if (position.Get_To_Move() == cc_w)
		result += 'w';
	else
		result += 'b';

	result += ' ';

	/* Third Field:  castling availability */

	int osize= *result;
	if (position.Get_Moved(1) == 0 &&
	    position[sq_e1] == pi_wK)
	{
		if (position.Get_Moved(2) == 0 &&
		    position[sq_h1] == pi_wR)
			result += 'K'; 
		if (position.Get_Moved(0) == 0 &&
		    position[sq_a1] == pi_wR)
			result += 'Q'; 
	}
	if (position.Get_Moved(4) == 0 &&
	    position[sq_e8] == pi_bK)
	{
		if (position.Get_Moved(5) == 0 &&
		    position[sq_h8] == pi_bR)
			result += 'k'; 
		if (position.Get_Moved(3) == 0 &&
		    position[sq_a8] == pi_bR)
			result += 'q'; 
	}

	if (*result == osize)
		result += '-';

	result += ' ';

	/* Fourth Field:  en passant target square */

	//Dbg_Int(position.Get_EP_Square());
	if (position.Get_EP_Square() == 0)
		result += '-';
	else
	{
		result += 'a' + File(position.Get_EP_Square()) - sq_a;
		if (Rank(position.Get_EP_Square()) == sq_4)
			result += '3';
		else
			result += '6';
	}

	result += ' ';

	/* Fifth Field:  halfmove clock */

	result += Format("%d", position.Get_Halfmove_Clock()); 

	result += ' ';

	/* Sixth Field:  fullmove number */

	result += Format("%d", position.Get_Previous_Move_Count() / 2 + 1);

	return result;
}

