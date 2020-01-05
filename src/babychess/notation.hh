#ifndef NOTATION_INCLUDED
#define NOTATION_INCLUDED

/* Chess notation. 
 */

#include "string.hh"
#include "move_name.hh"

/* Notation options. 
 */
enum
{nt_short             = (1 << 0),  /* Short notation                            */
 nt_pawn_sign         = (1 << 1),  /* Sign for pawn moves                       */
 nt_ep                = (1 << 2),  /* En passant                                */
 nt_check             = (1 << 3),  /* Check, checkmate                          */
 nt_short_capture     = (1 << 4),  /* Capture in short notation                 */ 
 nt_short_capture_end = (1 << 5),  /* Capture sign at the end in short notation */
 nt_short_pawn        = (1 << 6),  /* Sign for pawn captures in short notation  */ 
 nt_capture_name      = (1 << 7),  /* Name of captured piece in long notation   */ 
 nt_double_check      = (1 << 8),  /* Show double check using "++"              */

 nt_count= 9
};

class Notation
	:  public Pointed
{
public:

	int options; 
	/* nt_* constants. */

	enum {s_ep, 
	      s_castling, s_short_castling=s_castling, s_long_castling, 
	      s_P, 
	      s_capture= s_P+6, 
	      s_promotion, 
	      s_color, s_color_b, 
	      s_count};
	String strings[s_count];
	/* no newlines. */


	Notation() 
		/* Raw */ 
		{
		}
	Notation(char new_chars[10],
		 int new_options,
		 String new_ep,
		 String new_short_castling,
		 String new_long_castling);

	String Format_Move_Name(Move_Name) const;

	String Format_Piece_Type(int piece_type) const
		{
			assert (piece_type >= 0 && piece_type < 6);
			return strings[s_P + piece_type]; 
		}


//	void Reset();
	const String &operator [] (int i) const
		{
			assert (i >= 0 && i < s_count);
			return strings[i];
		}
	String &operator [] (int i) 
		{
			assert (i >= 0 && i < s_count);
			return strings[i];
		}

private: 

	void Append_Sign(String &string, 
			 int piece_type) 
		const
		{
			assert (piece_type >= 0 && piece_type < 6); 
			if ((piece_type != pt_P) || (options & nt_pawn_sign))
				string += //pieces[piece_type]; 
					Format_Piece_Type(piece_type);
		}
};

extern const int san_chars[26];
/* For each letter from A to Z, the piece type that corresponds to the
 * letter.  If the letter 'x' is a character for a piece in SAN, then
 * san_chars['x'-'a'] contains the piece type.  If the characters is
 * not used in SAN, then the character might correspond to a piece
 * type in another language.  Letters that do not match any languages
 * are indicated by 8. 
 */

Inline int San_Char(char c)
	/* Interface to SAN_CHARS. 
	 */
{
	if (c >= 'A' && c <= 'Z')
		c += ('a' - 'A');

	if (! (c >= 'a' && c <= 'z'))
		return 8;

	return san_chars[c - 'a'];
}


#endif /* ! NOTATION_INCLUDED */
