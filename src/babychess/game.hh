#ifndef GAME_INCLUDED
#define GAME_INCLUDED

/* See etc/GAME for more documentation. */

#include "dynamic.hh"
#include "generator.hh"
#include "pointer.hh"
#include "hash_table.hh"
#include "game_info.hh"
#include "variation.hh"
#include "notation.hh"

class Game
	:  public Pointed
{
public:

	Game();
	Game(const Game &);
	~Game(); 

	bool Is() const
		{
			return *location_stack != 0;
		}

	void New();
	/* New game in initial position.  May be open. 
	 */
	bool New(String fen);
	/* New game starting from FEN. */ 
	bool New(const Position &);
	/* New game from POSITION.  The position must be legal.  
	 */
	void Close();

	bool Append_Move(int index);
	/* To the end of current variation.  Location must be the end
	 * of the current variation. 
	 */
	bool Go_To_Start();
	/* Return TRUE when position changed.
	 */
	bool Go_Forward();
	/* Return TRUE when position changed.
	 */
	bool Go_To_End();
	/* Return TRUE when position changed.
	 */
	void Remove_End();

	bool Is_End();
	void Set_Overall_Comment(String comment)
		{
			variation_stack[-1]().Set_Comment(location_stack[-1], comment);
		}
	void Append_Comment(String comment)
		{
			Set_Overall_Comment(Get_Overall_Comment() + comment); 
		}

	String Get_Overall_Comment() const
		{
			return variation_stack[-1]().Get_Comment(location_stack[-1]); 
		}
	
	const Position &Get_Position() const
		{
			assert (Is()); 
			return position;
		}

	const Generator &Get_Generator() const 
		{
			assert (Is());
			return generator; 
		}

	const Dynamic <Position> &Get_Initial_Position() const
		/* Initial position.  Not defined if game begins from
		 * standard initial position. 
		 */
		{
			return initial_position; 
		}

	const Game_Info &Get_Game_Info() const
		{
			return game_info; 
		}
	Game_Info &Get_Game_Info()
		{
			return game_info; 
		}
	
	int Get_First_Move() const
		/* Which side moves first in the game. 
		 */
		{
			assert (Is());
			return (initial_position.Is() 
				? initial_position().Get_To_Move()
				: cc_w);
		}

	void Set_Game_Info(const Game_Info &new_game_info)
		{
			game_info= new_game_info; 
		}

	const Variation &Get_Main_Variation() const
		{
			return variation; 
		}
	Variation &Get_Current_Variation()
		{
			return variation_stack[-1](); 
		}
	const Variation &Get_Current_Variation() const
		{
			return variation_stack[-1](); 
		}
	int Get_Current_Location() const
		{
			return location_stack[-1]; 
		}
	Move_Name Get_Current_Move_Name(int current_location) const
		{
			return variation_stack[-1]().Get_Move_Name
				(current_location); 
		}
	void Append_Variation(int move_index);

	void operator = (const Game &); 
	int Get_Overall_Location() const
		{
			return variation_stack[-1]().Get_Previous_Move_Count() 
				+ location_stack[-1] 
				- variation.Get_Previous_Move_Count(); 
		}

	const Move &Get_Overall_Move(int i) const;

	Move_Name Get_Overall_Move_Name(int i) const;

	int Get_Overall_Length() const
		{
			return variation_stack[-1]().Get_Previous_Move_Count()
				+ variation_stack[-1]().Get_Length();
		}
	int Get_Variation_Depth() const
		{
			return *location_stack; 
		}
	bool Is_Overall_Variation_Move(int overall_location) const;

	bool Go_To_Overall_Location(int location);
	/* Return whether a variation was left.
	 */
	int Get_Current_Length() const
		{
			return variation_stack[-1]().Get_Length(); 
		}
	const Move &Get_Current_Move(int loc) const
		{
			return variation_stack[-1]()[loc];
		}
	void Remove_Variation();
	/* Remove current variation. 
	 */

	void Set_NAG(int nag);
	/* To last move in current variation.  Current location must
	 * not be 0. 
	 */
	bool Leave_Variation();
	/* Return whether a variation was left. 
	 */
	bool Go_Backward();
	bool Make_Main();

	bool Make_Parent();
	void Remove_Variation(int index);
	void Normalize(); 

	bool Append_Variation(); 
	/* Append a variation that replaces the next move in the current
	 * variation and enter the newly created variation.  Leaves
	 * the game non-normalized. 
	 */
	void Go_Backward_In_Current();
	/* Take back last move of current variation.
	 * Must have LOCATION_STACK[-1] > 0.
	 * Return TRUE when position changed.
	 * May make the position non-normalized.  Don't update the
	 * generator. 
	 */
	void Enter_Variation(int variation_index);
	/* Non-normalized.  Doesn't change the position. 
	 */
	void Play_Variation(int i);
	String Get_Overall_Move_Text(int i, const Notation &notation) const;
	/* The name of the move, followed by the glyph and "*" if the
	 * moves begins a subvariation. 
	 */
	String Format_Variation_Move(int variation_index, const Notation &) const;
	const Move &Get_Variation_Move(int i) const;
	void Get_Variation_Location(int overall_location,
				    int &variation, 
				    int &variation_location) 
		const;
	/* Given an OVERALL_LOCATION, return the index of the
	 * corresponding variation and the location within that
	 * variation. 
	 */
	int Get_Overall_NAG(int overall_location) const
		{
			int v, i;
			Get_Variation_Location(overall_location, v, i);
			return variation_stack[v]().Get_NAG(i);
		}
	String Format_Last_Move(const Notation &) const;
	const Vector <int> &Get_Location_Stack() const
		{
			return location_stack;
		}
	const Vector <Pointer <Variation> > &Get_Variation_Stack() const
		{
			return variation_stack; 
		}

private:

	Variation variation;

	Vector <int> location_stack;
	/* Position in the game.  
	 * LOCATION_STACK[i] is the position in VARIATION_STACK[i].
	 * For all i, 0 <= LOCATION_STACK[i] <= *VARIATION_STACK[i]. 
	 */

	Vector <Pointer <Variation> > variation_stack;
	/* *VARIATION_STACK == *LOCATION_STACK.
	 * For all i, VARIATION_STACK[i] != NULL. 
	 * VARIATION_STACK[0] == & VARIATION.  
	 * For i > 0, VARIATION_STACK[i] is a subvariation at position 
	 * LOCATION_STACK[i - 1] of VARIATION_STACK[i - 1]. 
	 */

	Position position;
	Generator generator;

	Dynamic <Position> initial_position;
	/* If not allocated, the game begins from the initial
	 * position.     
	 */
	Game_Info game_info; 

	void Reset_Position();
	/* Set POSITION to the game's initial position 
	 */
	void Go_Up();
	/* Leave the current variation.  
	 */

	static void Commentify(String &comment);
};

#endif /* ! GAME_INCLUDED */
