#ifndef RAW_PGN_INCLUDED
#define RAW_PGN_INCLUDED

/* An undecoded PGN game. 
 */

#include "string.hh"
#include "hash_table.hh"
#include "vector.hh"
//#include "game_info.hh"
#include "date.hh"

class Raw_PGN
{
private:

	Hash_Table tags;
	Vector <String> moves;

public:

	void Set_Tags(const Hash_Table /*<String>*/ &new_tags)
		{
			tags= new_tags;
		}

	void Clear_Moves()
		{
			moves.Free();
		}
	void Append_Move_Line(String line)
		{
			if (! moves.Set(*moves + 1))
				return;
			moves[-1]= line;
		}
	String Get_Tag(String tag) const
		{
			String result;
			if (! tags.Get(tag, result))
				return "";
			return result;
		}
	const Hash_Table &Get_Tags() const
		{
			return tags;
		}
	const Vector <String> &Get_Moves() const
		{
			return moves;
		}
	void Reset();

	void Normalize(); 

	int Get_Line_Count() const
		{
			return *moves;
		}
	const String &Get_Line(int i) const
		{
			return moves[i];
		}
};

#endif /* ! RAW_PGN_INCLUDED */
