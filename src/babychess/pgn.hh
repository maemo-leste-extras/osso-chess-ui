#ifndef PGN_INCLUDED
#define PGN_INCLUDED

/* Portable Game Notation.  Text notation for chess games. 
 *
 * See <http://www.clark.net/pub/pribut/standard.txt>. 
 */

/* PGN is "open" when "stream" is non-NULL. 
 */

#include "game.hh"
#include "raw_pgn.hh"
#include "stream.hh"
#include "parser.hh"

class PGN
{
public:

	static bool Parse(Stream &stream, 
			  Game &game,
			  String &carry); 

	static bool Write(Stream &stream,
			  const Game &game);
	static bool Parse_Raw(Stream &stream,
			      Raw_PGN &raw,
			      String &carry);
	static bool Write_Raw(Stream &stream,
			      const Raw_PGN &raw_pgn); 
	static bool Read_Raw(const Raw_PGN &raw,
			     Game &game); 
	static void Write_Variation(const Variation &variation);
	static void Open(Stream &stream);
	static void Close();

private:

	class PGN_Tag_Writer
		:  public Hash_Table::Callback
	{
	public:
	
		void Call(String key, String value);
	};

	enum Last_Word {lw_nothing, lw_close, lw_open, lw_move,
			lw_number};

	enum {line_length= 80}; 
	/* As defined by the PGN standard. 
	 */

	/* Set and cleared in the public
	 * functions and used in the other functions. 
	 */
	static Pointer <Stream> stream;
	/* Set in Write(). 
	 */
	static int char_count;
	static String rem_word;
	
	static bool 
	Write_Tags(const Game &game);
	/* Including the empty line after the tags. 
	 */

	static void
	Write_Word(String word);
	/* Write a space or a newline, respecting PGN line length, and
	 * WORD. 
	 */
	static bool
	Write_Comment(String comment);
	/* Return whether there was a comment. 
	 */

	static void
	Write_Variations(const Vector <Dynamic <Variation> > &variations);

	static bool
	Parse_Header(Hash_Table &tags,
		     String &carry);
	/* Return whether at least one tag was read. 
	 */
	static void
	Parse_Move(String move_text,
		   Game &game,
		   Parser &parser);
	static bool
	Parse_Moves(const Vector <String> &moves,
		   Game &game); 
	static bool
	Parse_Raw(Raw_PGN &raw,
		  String &carry);
	static bool
	Write_Raw(const Raw_PGN &raw_pgn); 
	static bool 
	Write_Raw_Tags(const Hash_Table &tags);

	static void Flush();
};

#endif /* ! PGN_INCLUDED */
