#include "simple_notation.hh"

String
Simple_Notation(const Move &move)
{
	char english_chars[]= {'n', 'b', 'r', 'q'};
	char r[6];

	r[0]= 'a' + File(move.from);
	r[1]= '1' + Rank(move.from);
	r[2]= 'a' + File(move.to);
	r[3]= '1' + Rank(move.to);
	if (move.promotion.Is())
	{
		r[4]= english_chars[move.promotion.Get_Type() - 1];
		r[5]= '\0';
	}
	else
		r[4]= '\0';
	return r;
}

