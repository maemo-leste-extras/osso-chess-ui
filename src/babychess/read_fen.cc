#include "read_fen.hh"

#include "parser.hh"
//#include "error.hh"
#include "format.hh"
//#include "dbg.hh"
#include "notation.hh"
#include "printify_char.hh"

#include <libintl.h>
#include <stdio.h>

bool Read_FEN(String    text,
	      Position &position)
{
	//Dbg_Function();

	if (!text)
	{
		//Error::Set("Empty String");
		return false;
	}

	Parser parser(text);

	/* first field */
	parser.Skip_Space();
	String first_field= parser.Skip_Word();
	
	int file= 0, rank= 7;
	for (int i= 0;  i < *first_field;  ++i)
	{
		char c= first_field[i];

		switch (c)
		{
		case '1' ... '8':
		{
			int n= c - '0';
			if (file + n > 8)
			{
				//Error::Set(gettext("More than eight squares on one row"));
				return false;
			}
			for (int j= 0;  j < n;  ++j)
				position/*.Set_Piece*/[Make_Square(file + j, rank)]= Piece(false);
			file += n;
			break;
		}
		case 'a' ... 'z':
         	case 'A' ... 'Z':
		{
			if (file > 7)
			{
				//Error::Set(gettext("More than eight squares on one row"));
				return false;
			}
			int color= c >= 'A' && c <= 'Z' ? cc_w : cc_b;
			int piece_type= San_Char(c);
			if (piece_type == 8)
			{
				String printified= Printify_Char(c);
				/*Error::Set(Format(gettext("Invalid character \"%s\" in the first field"),
						  printified()));*/
				return false;
			}

			Piece piece= Piece(color, piece_type);
			int square= Make_Square(file, rank);
			position/*.Set_Piece*/[square]= piece;
			if (piece_type == pt_K)
			{
				position.Set_King_Square(color, square);
			}
			++ file;
			break;
		}

		case '/':
		{
			if (file < 7)
			{
				//Error::Set(gettext("Less than eight squares on one row"));
				return false;
			}
			if (rank == 0)
			{
				//Error::Set(gettext("More than eight rows"));
				return false;
			}
			file= 0;
			-- rank;
			break;
		}
		
	        default:
		{
			String printified= Printify_Char(c);
			/*Error::Set(Format(gettext("Invalid character \"%s\" in the first field"),
					  printified()));*/
			return false;
		}
	        }
	}

	assert(file <= 8);
	if (rank > 0)
	{
		//Error::Set(gettext("Less than eight rows"));
		return false;
	}

	if (file < 8)
	{
		//Error::Set(gettext("Less than eight squares on one row"));
		return false;
	}

	/* second field */
	parser.Skip_Space();
	String second_field= parser.Skip_Word();
	position.Set_To_Move(second_field[0] == 'w' ? cc_w : cc_b); 

	/* third field  (castlings) */
	parser.Skip_Space();
	String third_field= parser.Skip_Word();
	for (int i= 0; i < 6; ++i)
		position.Set_Moved(i, 1);
	for (int i= 0;  i < *third_field;  ++i)
	{
		char c= third_field[i];
		if (c == 'K'                   && 
		    position[sq_e1] == pi_wK   &&
		    position[sq_h1] == pi_wR)
		{
			position.Set_Moved(1, 0);
			position.Set_Moved(2, 0);
		}
		else if (c == 'Q'                 &&
			 position[sq_a1] == pi_wR &&
			 position[sq_e1] == pi_wK)
		{
			position.Set_Moved(1, 0);
			position.Set_Moved(0, 0);
		}
		else if (c == 'k'                 &&
			 position[sq_e8] == pi_bK &&
			 position[sq_h8] == pi_bR)
		{
			position.Set_Moved(4, 0);
			position.Set_Moved(5, 0);
		}
		else if (c == 'q'                  &&
			 position[sq_a8] == pi_bR  &&
			 position[sq_e8] == pi_bK)
		{
			position.Set_Moved(4, 0);
			position.Set_Moved(3, 0);
		}
	}

	/* fourth field:  ep capture square (eg "e6") */
	parser.Skip_Space();
	String fourth_field= parser.Skip_Word();
	//Dbg_Str(fourth_field());
	char c= fourth_field[0];
	position.Get_EP_Square()= ((c >= 'a' && c <= 'h') 
				   ? Make_Square(c - 'a', position.Get_To_Move() == cc_w ? 4 : 3) 
			     : 0);
	//Dbg_Int(position.Get_EP_Square());

	/* Fifth field:  halfmove clock.  (ignored)
	 */
	parser.Skip_Space();
	String hm_text= parser.Skip_Word();
	int new_hm;
	if (1 != sscanf(hm_text(), "%d", & new_hm))
		new_hm= 0;
	else
		if (new_hm < 0)
			new_hm= 0;
	position.Set_Halfmove_Clock(new_hm); 

	/* Sixth field:  fullmove number
	 */
	parser.Skip_Space();
	String sixth_field= parser.Skip_Word();
	int fullmove;
	if (1 != sscanf(sixth_field(),
		      "%d",
		      & fullmove))
		fullmove= 1;

	position.Set_Previous_Move_Count( (fullmove - 1) * 2 + position.Get_To_Move());

	position.Update_Material();
	return position.Set_Legal(); 
}
