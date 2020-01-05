#include "notation.hh"

//#include "dbg.hh"
#include "format.hh"

String Notation::Format_Move_Name(Move_Name move_name)
	const
{
	/*Dbg_Function();
	Dbg_Int(move_name.from_file);
	Dbg_Int(move_name.to_file);
	*/

	String result;

	switch (move_name.special)
	{
	case 2:
		result= strings[s_short_castling];
		break;

	case 3:
		result= strings[s_long_castling];
		break;

	default:

		Append_Sign(result, move_name.piece_type);

		if (! (options & nt_short))
			/* Long notation */ 
		{
			result += Format("%c%c", 'a'+move_name.from_file, '1'+move_name.from_rank);
//			result += Format_Square(Make_Square(move_name.from_file, move_name.from_rank));

			if (move_name.capture == 0)
				result += '-';
			else
			{
				if (strings[s_capture])
					result += strings[s_capture]; 
			}

			if (move_name.capture != 0 && (options & nt_capture_name))
				Append_Sign(result, move_name.capture - 1);
			result += Format("%c%c", 'a'+move_name.to_file, '1'+move_name.to_rank);
//			result += Format_Square(Make_Square(move_name.to_file, move_name.to_rank));
		}
		else
			/* Short notation */ 
		{
			if (move_name.piece_type != pt_P)
			{
				if (move_name.precision % 2)
					result += 'a' + move_name.from_file;
				if (move_name.precision / 2)
					result += '1' + move_name.from_rank;
				if (move_name.capture != 0 
				    && strings[s_capture]     
				    && (options & nt_short_capture) 
				    && (! (options & nt_short_capture_end)))
					result += strings[s_capture];
			}
			else
			{
				if (move_name.capture != 0)
				{
					result += 'a' + move_name.from_file;
					if ((options & nt_short_pawn) 
					    && strings[s_capture]           
					    && (options & nt_short_capture) 
					    && ! (options & nt_short_capture_end))
						result += strings[s_capture];
				}
			}

			result += Format("%c%c", 'a'+move_name.to_file, '1'+move_name.to_rank); 
//			result += Format_Square(Make_Square(move_name.to_file, move_name.to_rank));

			if (move_name.capture != 0 
			    && strings[s_capture]        
			    && (options & nt_short_capture) 
			    && (options & nt_short_capture_end) 
			    && (move_name.piece_type != pt_P || (options & nt_short_pawn)))
				result += strings[s_capture];
		}

		if ((options & nt_ep) && move_name.special == 1)
		{
			result += ' ';
			result += strings[s_ep];
		}
		if (move_name.special >= 4)
		{
			if (strings[s_promotion])
				result += strings[s_promotion];
			Append_Sign(result, move_name.special - 3);
		}
	}

	if (options & nt_check)
		if (move_name.check)
		{
			if (move_name.force)
				result += '#';
			else
			{
				if (options & nt_double_check && move_name.check == 2)
					result += "++";
				else
					result += '+'; 
			}
		}

	//Dbg_Str(result()); 

	return result;
}


/* S= SAN, F= french, D= german */
const int san_chars[26]=
/**/{8,2,1,4,8,2,8,8,8,8,5,2,8,1,8,0,4,3,1,3,8,8,8,8,8,8};
/*   A B C D E F G H I J K L M N O P Q R S T U V W X Y Z       (letter)
 *     S F D   F         S     S   S S S D F                   (language)
 */

Notation::Notation(char new_chars[10],
		   int new_options,
		   String new_ep,
		   String new_short_castling,
		   String new_long_castling)
	:  options(new_options)
{
	assert (new_ep.Is()); 
	assert (new_short_castling.Is());
	assert (new_long_castling.Is()); 
	assert (s_count == 13);

	strings[0]= new_ep;
	strings[1]= new_short_castling;
	strings[2]= new_long_castling;
			
	for (int i= 0;  i < 10;  ++i)
	{
		if (new_chars[i])
			strings[i+3]= new_chars[i];
	}
}
