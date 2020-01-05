#include "generator.hh"
#include <glib.h>

bool Generator::New(const Position &new_position,
		    bool no_names)
{
	position= new_position;

	/* Generate legal moves */ 

	Position tmp_position= new_position;
	moves.Free();
	Legal::Generate(tmp_position, moves, true, false);

//	Dbg_Int_(*moves); 

	flags= Legal::flags;

	names.Free(); 

	if (no_names)
		return true; 

	/* Generate move names */

	//Dbg("\tGenerating move names.");

	if (! names.Set(*moves))
		return false;

	for (int i= 0;  i < *moves;  ++i)
	{
		names[i].piece_type= tmp_position/*.Get_Piece*/[moves[i].from].Get_Type();
		Piece piece= tmp_position/*.Get_Piece*/[moves[i].to];
		names[i].capture= piece.Is() ? 1 + piece.Get_Type() : 0;
		names[i].from_file= File(moves[i].from);
		names[i].from_rank= Rank(moves[i].from);
		names[i].to_file= File(moves[i].to);
		names[i].to_rank= Rank(moves[i].to);

		if (moves[i].promotion.Is())
			names[i].special= 3 + moves[i].promotion.Get_Type();
		else if (moves[i].ep_square)
		{
			names[i].special= 1;
			names[i].capture= 1;
		}
		else if (moves[i].castling)
			names[i].special= moves[i].castling % 2 + 2;
		else
			names[i].special= 0;
      
		if (tmp_position/*.Get_Piece*/[moves[i].from].Get_Type() != pt_P)
		{
			bool d= false, dl= false, dr= false;
			for (int j= 0;  j < *moves;  ++j)
			{
				if (j != i                     &&
				    moves[i].to == moves[j].to   &&
				    (tmp_position/*.Get_Piece*/[moves[i].from] ==
				     tmp_position/*.Get_Piece*/[moves[j].from]))
				{
					d= true;
					if (File(moves[i].from)==File(moves[j].from))
						dl= true;
					if (Rank(moves[i].from)==Rank(moves[j].from))
						dr= true;
				}
			}
			if (d)
			{
				if (dl)
				{
					if (dr)
						names[i].precision= 3;
					else
						names[i].precision= 2;
				}
				else
					names[i].precision= 1;
			}
			else
				names[i].precision= 0;
		}

		tmp_position << moves[i];
		int check= tmp_position.In_Check(true);
		assert (check == 0 || check == 1 || check == 2);
		names[i].check= check;

		Vector <Move> tmp_moves;
		Legal::Generate(tmp_position, tmp_moves, true, true);
		names[i].force= *tmp_moves ? 0 : 1;

		tmp_position >> moves[i];
	}

	return true; 
}

int Generator::Get_Square_Move(int from, 
			       int to)
	const
{
	for (int i= 0;  i < **this;  ++i)
	{
		if (from == Get_Move(i).from && to == Get_Move(i).to)
			return i; 
	}
	return -1;
}

int
Generator::Get_e2e4_Notation_Move(String move_text) 
	const
{
	if (move_text[0] < 'a' || move_text[0] > 'h'  ||
	    move_text[1] < '1' || move_text[1] > '8'  ||
	    move_text[2] < 'a' || move_text[2] > 'h'  ||
	    move_text[3] < '1' || move_text[3] > '8')
		return -1;

	int
		from=  Make_Square(move_text[0] - 'a', move_text[1] - '1'),
		to=    Make_Square(move_text[2] - 'a', move_text[3] - '1');
  
	int index= Get_Square_Move(from, to);

	switch (move_text[4])
	{
	case 'n':  case 'N':  index += 3;  break;
	case 'b':  case 'B':  index += 2;  break;
	case 'r':  case 'R':  index += 1;  break;
	}

	if (index >= **this)
		return -1;

	return index; 
}


void Generator::Free() 
{
	assert (Is());
	position= NULL;
	moves= 0;
	names= 0; 
}
