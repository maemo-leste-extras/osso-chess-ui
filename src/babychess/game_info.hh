#ifndef GAME_INFO_INCLUDED
#define GAME_INFO_INCLUDED

#include "hash_table.hh"

class Game_Info
	:  public Pointed
{
public:
	
	enum Result {re_white= 0, re_draw, re_black, re_unfinished}; 

	String event, site, round, white, black; 

	Result result; 
	/* The game result is independent from the game moves.
	 * Example:  The game might not end in a checkmate, and the
	 * result might be "1-0". 
	 */

	int date_year, date_month, date_day; 
	/* Month:  0-based. 
	 * Day:    1-based. 
	 * -1 when unknown for each.  (e.g. the year may be known but
	 * not the month/day. 
	 */

	Hash_Table tags;
	/* Additional tags. 
	 */

	/* The FEN tag is not part of Game_Info.  It is part of the
	 * game. 
	 */

	void Reset();

	void Set(const Hash_Table &all_tags);
	/* ALL_TAGS contains all tags, including those that
	 * are stored as a string and Result in Game_Info.
	 * Ignore "FEN" and "SetUp" tags. 
	 */
	String Get_Result_Text() const
		{
			return Get_Result_Text(result); 
		}
	String Format_Date() const;
	/* In the format mandated by the PGN standard */ 

	String Format_Date_Locally() const
	/* Using the locale */
		{
			return Format_Date_Locally(date_year, date_month, date_day); 
		}

	static String Get_Result_Text(Result result)
		{
			assert (result >= 0 && result <= 3);
			return result_text[result]; 
		}
	static void Parse_Date(String date, int &year, int &month, int &day);
	/* DATE is in PGN-format.  Parse it and put the result in the
	 * INTs, as described in the comments of the members of
	 * Game_Info. 
	 */
	static String Format_Date_Locally(int year, int month, int day);

private:

	static const char *result_text[4];
};

#endif /* ! GAME_INFO_INCLUDED */ 
