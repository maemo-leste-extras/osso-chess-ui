#include "raw_pgn.hh"

void Raw_PGN::Normalize()
{
	if (! Get_Tag("Event").Is())
		tags.Set("Event", "?");
	if (! Get_Tag("Site").Is())
		tags.Set("Site", "?");
	if (! Get_Tag("Date").Is())
		tags.Set("Date", "?");
	if (! Get_Tag("Round").Is())
		tags.Set("Round", "?");
	if (! Get_Tag("White").Is())
		tags.Set("White", "?");
	if (! Get_Tag("Black").Is())
		tags.Set("Black", "?");
	if (! Get_Tag("Result").Is())
		tags.Set("Result", "*"); 
}

void Raw_PGN::Reset()
{
	/* Keep parallel to Game_Info::Reset(). 
	 */

	tags.Free();
	moves.Free();
	tags.Set("Date", 
		 Format_Date(Get_Date(),
			     "%Y.%m.%d"));
}


