#include "pgn.hh"

#include "string.hh"
#include "text_utils.hh"
#include "parse_move.hh"
#include "read_line.hh"
//#include "error.hh"
#include "fen.hh"
#include "format.hh"
#include "notation.hh"
#include "nag.hh"
#include "san.hh"

#include "ui/interface.hh"

#include <libintl.h>
#include <ctype.h>

Pointer <Stream> PGN::stream;

int PGN::char_count;
String PGN::rem_word; 
bool error_found=false; // true when found an error during parsing

bool PGN::Write_Raw_Tags(const Hash_Table &tags)
{
	PGN_Tag_Writer writer;
	tags.Iterate(writer);

	/* End-of-header empty line. 
	 */
	stream().Write_Char('\n');

	return true;
}

bool PGN::Write_Tags(const Game &game)
{
	assert (game.Get_Game_Info().result >= 0 &&
		game.Get_Game_Info().result < 4);

	String date_text= game.Get_Game_Info().Format_Date();

	int difficulty = settings_get_int(SETTINGS_LEVEL);

	fprintf(stream()(),
		"[Event \"%s\"]\n"
		"[Site \"%s\"]\n"
		"[Date \"%s\"]\n"
		"[Round \"%s\"]\n"
		"[White \"%s\"]\n"
		"[Black \"%s\"]\n"
        "[Difficulty \"%d\"]\n"
		"[Result \"%s\"]\n",
		game.Get_Game_Info().event.Is() ? game.Get_Game_Info().event() : "?",
		game.Get_Game_Info().site .Is() ? game.Get_Game_Info().site () : "?",
		date_text(),
		game.Get_Game_Info().round.Is() ? game.Get_Game_Info().round() : "?",
		game.Get_Game_Info().white.Is() ? game.Get_Game_Info().white() : "?",
		game.Get_Game_Info().black.Is() ? game.Get_Game_Info().black() : "?",
		difficulty,
		Game_Info::Get_Result_Text(game.Get_Game_Info().result)()); 

	/* FEN tag. 
	 */
	if (game.Get_Initial_Position().Is())
	{
		String fen= FEN(game.Get_Initial_Position()()); 
		fprintf(stream()(),
			"[FEN \"%s\"]\n"
			"[SetUp \"1\"]\n",
			fen()); 
	}

	/* Other tags and empty line. 
	 */
	Write_Raw_Tags(game.Get_Game_Info().tags); 

	return true;
}

void PGN::Write_Word(String word)
{
//	Dbg_Function_();
//	Dbg_Str_(word());

	assert (word.Is());

	if (rem_word.Is()         &&
	    rem_word[-1] != '('   &&
	    word[0]      != ')')
		/* Words cannot stick together. */
	{
		Flush();
		rem_word= word;
	}
	else
	{
		rem_word += word;
	}
}

bool PGN::Write_Comment(String comment)
{
	if (! comment.Is())
		return false;

	Write_Word(Format("{%s}", comment()));
	return true; 
}

void PGN::Write_Variations(const Vector <Dynamic <Variation> > &variations)
{
	for (int i= 0;  i < *variations;  ++i)
	{
		const Variation &variation= variations[i]();

		Write_Word("(");

		Write_Variation(variation);

		Write_Word(")");
	}
}

void PGN::Write_Variation(const Variation &variation)
{
	Write_Comment(variation.Get_Comment(0));

	bool previous_obstructions= false;
	/* Whether comments or subvariations were written after the
	 * last move.  If true, the move number must be written even
	 * in the case of a black move. 
	 */

	for (int i= 0;  i < *variation;  ++i)
	{
		/* Move number */

		int overall_move_number= i + variation.Get_Previous_Move_Count();
		if (overall_move_number % 2 == 0)
		{
			Write_Word(Format("%d.",
					  overall_move_number / 2 + 1));
		}
		else if (i == 0 || previous_obstructions)
			/* first move is black */
		{
			Write_Word(Format("%d...",
					  overall_move_number / 2 + 1));
		}
			

		/* The move */  
		String move_name= san.Format_Move_Name(variation.Get_Move_Name(i));
		Write_Word(move_name);

		/* NAG */

		int nag= variation.Get_NAG(i);
		if (nag != 0)
			Write_Word(Format("$%d", nag)); 

		/* Comment */ 

		previous_obstructions= Write_Comment(variation.Get_Comment(i + 1));

		/* Variations */ 

		if (variation.Have_Variations(i))
		{
			Write_Variations(variation.Get_Variations(i));
			previous_obstructions= true;
		}
	}
}

bool PGN::Write(Stream &new_stream,
		const Game &game)
{
	stream= new_stream; 

	/* Tags */
	if (! Write_Tags(game))
		return false;

	/* Moves */
	char_count= 0;
	/* characters on one line.  Lines in the PGN file must have a limited 
	 * length. 
	 */
	rem_word.Free();
	
	Write_Variation(game.Get_Main_Variation());
	Write_Word(game.Get_Game_Info().Get_Result_Text()); 
	Flush();
	new_stream.Write_Char('\n'); /* end of line */ 

	/* empty line */
	new_stream.Write_Char('\n');

	stream= NULL;
	return true; 
}

bool PGN::Parse_Header(Hash_Table &tags,
		  String &carry)
{
	int tag_count= 0;
	/* Corresponds to *TAGS, but count also tags that could not be
	 * parsed.  
	 */

	bool wrote_tags= false;

	while (true)
		/* Read one line  */
	{
		String line;
		if (carry.Is())
		{
			line= carry;
			carry= "";
		}
		else
		{
			if (! Read_Line(stream(), line))
			{
				return wrote_tags;
			}				
			if (! line.Is())
			{
				if (tag_count != 0)
				{
					return true;
				}
			}
		}

		if (line.Is())
		{
			Parser parser(line);
			parser.Skip_Space();
			if (parser.End())
			{
				if (tag_count != 0)
					return true;
			}
			else 
			{
				if (parser.Skip_Char() == '[')
				{
					++ tag_count;
					parser.Skip_Space();
					String name= parser.Skip_Word();
					if (name.Is())
					{
						parser.Skip_Space();
						String value;
						if (parser.Skip_String(value))
						{
							tags.Set(name, value);
							wrote_tags= true;
						}
						else
						{
							if (value.Is())
							{
								tags.Set(name, value); 
								wrote_tags= true;
							}
							else
							{
								String word= parser.Skip_Word_Excluding
									(']');
								if (word.Is())
								{
									tags.Set(name, word); 
									wrote_tags= true;
								}
							}
						}
					}
				}
				else
				{
					carry= line;
					return tag_count != 0;
				}
			}
		}
	}
}

void PGN::
Parse_Move(String word,
	   Game &game,
	   Parser &parser)
{
	assert (word.Is());

	/* Ignore end-of-game markers */
	if (word == "1-0"      ||
	    word == "0-1"      ||
	    word == "1/2"      ||
	    word == "1/2-1/2"  ||
	    word == "*")
		goto end;

	/* Move consists of:
	 *      - move number:  "1.", "2...", ""
	 *      - move in SAN
	 *      - textual NAG:  "", "!", ...
	 */
	{
		int i= 0;
		while (isdigit(word[i]))  ++i;
		while (word[i] == '.')     ++i;

		if (i == *word)
			goto end;
		int j= i;
		while (word[j] &&
		       word[j] != '!' &&
		       word[j] != '?')
			++j;
		
		int sm= ::Parse_Move(word.Sub(i, j - i),
				     game.Get_Generator());
		if (-1 != sm)
		{
			game.Append_Move(sm);
		}
		if (j < *word)
		{
			int nag= Parse_NAG(word.Sub(j, *word - j));
			game.Set_NAG(nag);
		}
	}

 end:

	parser.Skip(*word);
	parser.Skip_Space();
}

bool PGN::Parse_Moves(const Vector <String> &moves,
		      Game &game)
{
	//Dbg_Function();
//	Dbg_Int(*moves); 
	if (*moves == 0)
	{
		//Error::Set(gettext("Game is empty"));
		return false; 
	}

	bool in_comment= false;

	for (int i= 0;  i < *moves;  ++i)
	{
		String line= moves[i];

		if (! (line.Is() || in_comment))
			/* empty line */
			return true;
		if (line[0] != '%')
		{
			Parser parser(line);
			do
			{
				if (in_comment)
				{
					game.Get_Current_Variation().Append_Comment
						(game.Get_Current_Variation().Get_Length(), "\n");
					String comment= parser.Skip_Excluding('}');
					game.Get_Current_Variation().Append_Comment
						(game.Get_Current_Variation().Get_Length(), 
						 comment);
					if (parser.Next_Char() == '}')
					{
						in_comment= false;
						parser.Skip_Char();
					}
				}

				if (! in_comment) 
				{
					parser.Skip_Space();

					if (parser.Next_Char() == '{')
					{
						parser.Skip_Char();
						String comment= parser.Skip_Excluding('}');
						game.Get_Current_Variation().Append_Comment
							(game.Get_Current_Variation().Get_Length(),
							 comment);
						if (parser.Next_Char() == '}')
						{
							parser.Skip_Char();
						}
						else
						{
							in_comment= true; 
						}
					}
					else if (parser.Next_Char() == '(')
					{
						/* take back last move. */
						if (game.Get_Current_Location() == 0)
							return false;

						game.Go_Backward_In_Current(); 

						if (! game.Append_Variation())
							return false;
						parser.Skip_Char();
						parser.Skip_Space();
					}
					else if (parser.Next_Char() == ')')
					{
						game.Leave_Variation();
						bool f= game.Go_Forward();
                        
                        if (!f)
                            {
                                error_found=true;
                                return false;
                            }

						parser.Skip_Char();
						parser.Skip_Space();
					}
					else if (parser.Next_Char() == '$')
					{
						parser.Skip_Char();
						String word= parser.Skip_Number();
						int nag;
						if (1 == sscanf(word(), "%d", & nag))
						{
							if (game.Get_Current_Location() != 0)
								game.Set_NAG(nag);
						}
						parser.Skip_Space();
					}
					else /* move name */
					{
						String word= parser.Next_Word_Excluding("{()"); 
						if (word.Is())
						{
							Parse_Move(word,
								   game, 
								   parser);
						}
					}
				}
			} while (! parser.End());
		}
	}
	game.Normalize();
	return true;
}

bool PGN::Parse(Stream &new_stream, 
		Game &game,
		String &carry)
{
//	Dbg_Function_();

	stream= & new_stream;

	Raw_PGN raw;
	bool have_header= Parse_Raw(raw, carry); 
	stream= NULL;

	String fen= raw.Get_Tag("FEN");

	/* Read game settings from chess saved file */
	AppUIData *app_ui_data = get_app_data()->app_ui_data;
	if (app_ui_data && app_ui_data->game_loading) {
		String white = raw.Get_Tag("White");
		String black = raw.Get_Tag("Black");
		String difficulty = raw.Get_Tag("Difficulty");
		if (white.Is() && black.Is() && (strcmp(white(), "?") != 0) && (strcmp(black(), "?") != 0)) {
			if ((strcmp(white(), CHESS_COMPUTER_NAME) == 0) || (strcmp(black(), CHESS_COMPUTER_NAME) == 0))
				app_ui_data->opponent_type_human = FALSE;
			else
				app_ui_data->opponent_type_human = TRUE;
			settings_set_bool(SETTINGS_OPPONENT_HUMAN, app_ui_data->opponent_type_human);
			
			if (app_ui_data->opponent_type_human)
			{
				settings_set_int(SETTINGS_LEVEL, chess_opponent_human);
			}
			else
			{
				/* Rama - Bug#98764 */
				if( (0 > atoi(difficulty())) ||  (5 < atoi(difficulty())) ) {
					error_found = true;
					settings_set_int(SETTINGS_LEVEL, 0);
					app_ui_data->difficulty_level = 0;
				}
				else {
					settings_set_int(SETTINGS_LEVEL, atoi(difficulty()));
					app_ui_data->difficulty_level = atoi(difficulty());
				}
			}
			
			if (strcmp(white(), CHESS_PLAYER1_NAME) == 0)
				app_ui_data->player_color_white = TRUE;
			else
				app_ui_data->player_color_white = FALSE;
			settings_set_bool(SETTINGS_PLAYER_COLOR, app_ui_data->player_color_white);
		}
#if 0
		else {	/* Rama - Bug#98764 */
			error_found = true;
			app_ui_data->opponent_type_human = FALSE;
			app_ui_data->difficulty_level = 0;
			app_ui_data->player_color_white = TRUE;
			
			settings_set_bool(SETTINGS_OPPONENT_HUMAN, app_ui_data->opponent_type_human);
			settings_set_int(SETTINGS_LEVEL, app_ui_data->difficulty_level);
			settings_set_bool(SETTINGS_PLAYER_COLOR, app_ui_data->player_color_white);

		}
#endif
	}

//	Dbg_Str_(fen());

	if (fen.Is())
	{
		if (! game.New(fen))
			return false;
	}
	else
		game.New();

	/* Rama - Bug#98764 */
	if(error_found) {
		return false;
	}

	if (! Parse_Moves(raw.Get_Moves(), game))
	{
		if (error_found || ! have_header)
			return false; 
	}

	return true; 
}

bool PGN::Parse_Raw(Raw_PGN &raw,
		    String  &carry)
{
	Hash_Table tags;

	bool have_header= Parse_Header(tags, carry); 
	raw.Set_Tags(tags);
	raw.Normalize();
	raw.Clear_Moves();
	int brace_level= 0;

	do
	{
		String line;
		if (carry.Is())
		{
			line= carry;
			carry= "";
		}
		else
		{
			if (! Read_Line(stream(), line))
			{
				return have_header || raw.Get_Moves().Is(); 
			}
		}
		int index= 0;
		while (isspace(line[index]))
			++ index;
		if (index == *line && brace_level == 0) /* line is empty */ 
		{
			return have_header || raw.Get_Moves().Is(); 
		}
		if (line[index] == '[' && brace_level == 0)
		{
			carry= line;
			return true;
		}
		while (index < *line)
		{
			if (line[index] == '{')
				++ brace_level;
			if (line[index] == '}')
			{
				if (brace_level > 0)
					-- brace_level;
			}
			++ index;
		}
		raw.Append_Move_Line(line);
	}
	while (true);
}

bool PGN::Read_Raw(const Raw_PGN &raw,
		   Game &game)
{
	String fen= raw.Get_Tag("FEN");
	if (fen.Is())
	{
		if (! game.New(fen))
			game.New(); 
	}
	else
		game.New();

	Parse_Moves(raw.Get_Moves(), game); 
	/* If Parse_Moves() returns FALSE, no moves were in the
	 * game.  That's OK because a PGN game may consist of only
	 * headers. 
	 */

	return true; 
}

bool
PGN::Write_Raw(const Raw_PGN &raw_pgn)
{
	if (! Write_Raw_Tags(raw_pgn.Get_Tags()))
		return false;

	for (int i= 0;  i < raw_pgn.Get_Line_Count();  ++i)
	{
		if (! stream().Write_Line(raw_pgn.Get_Line(i)()))
			return false;
		if (! stream().Write_Char('\n'))
			return false;
	}

	return stream().Write_Char('\n');
}

void PGN::PGN_Tag_Writer::Call(String key, String value)
{
	fprintf(stream()(),
		"[%s \"%s\"]\n",
		key(),
		value()); 
}

bool PGN::Parse_Raw(Stream &new_stream,
	       Raw_PGN &raw,
	       String &carry)
{
	stream= new_stream;
	bool result= Parse_Raw(raw, carry);
	stream= NULL;
	return result; 
}

bool
PGN::Write_Raw(Stream &new_stream,
	       const Raw_PGN &raw_pgn)
{
	stream= new_stream;
	bool result= Write_Raw(raw_pgn);
	stream= NULL;
	return result; 
}

void PGN::Flush()
{
	if (! rem_word.Is())  return;
	
	if (char_count && (char_count + *rem_word + 1 > line_length))
	{
//		Dbg_("\tnewline");
		stream().Write_Char('\n');
		char_count= 0;
	}
	else
	{
		if (char_count != 0)
		{
//			Dbg_("\tspace");
			stream().Write_Char(' ');
			char_count += 1;
		}
	}
	char_count += *rem_word;
	stream().Write_String(rem_word);
}

void PGN::Open(Stream &new_stream)
{
	assert (! stream.Is());

	stream= & new_stream;
}

void PGN::Close()
{
	assert (stream.Is());

	Flush(); 

	stream= NULL; 
}
