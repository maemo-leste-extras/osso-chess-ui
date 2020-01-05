#ifndef POSITION_INCLUDED
#define POSITION_INCLUDED

/* Chess position. 
 *
 * The flag LEGAL determines the mode:
 *    - TRUE:   legal mode:    The position is legal and only legal
 *                             moves can be made.  
 *    - FALSE:  illegal mode:  any changes can be made to the
 *                             position. 
 *
 * IN_CHECK, MATERIAL and KING_SQUARE are only consistent in legal
 * mode.  
 *
 * Use Get_Legal() to get the mode, and Set_Legal()/Set_Illegal() to
 * change the mode. 
 */

#include "square.hh"
#include "piece.hh"
#include "move.hh"
#include "pointer.hh"
#include "material.hh"

class Position
	:  public Pointed
{
public:

	Position() 
		/* empty */
		{
			Init_Outside(); 
			
			legal= false;

			to_move= 0;
			moved[1]= 0;
			moved[4]= 0;

			Empty();
		}
	Position(int /* dummy */ ) 
		/* From Initial position */
		{
			Init_Outside();  
			legal= true;
			Reset(); 
		}
	Position(int, int) /* raw */
		{
			Init_Outside(); 
		}
	~Position() 
		{
		}
	Piece &operator [] (int square) 
		{
			return pieces.pieces[square];
		}
	Piece operator [] (int square) const
		{
			return pieces.pieces[square];
		}

	const Piece *Get_Pieces() const 
		{
			return pieces.pieces; 
		}
	void Set_Moved(int index, int new_moved)
		{
			assert (!legal);
			assert(0 <= index && index < 6);
			moved[index]= new_moved;
		}
	int Get_Moved(int index) const
		{
			assert(0 <= index && index < 6);
			return moved[index];
		} 
	void Set_King_Square(int chess_color, int square)
		/* Must be in illegal mode.  KING_SQUARE will be
		 * verified on entering legal mode. 
		 */
		{
			assert (!legal);
			assert(chess_color == cc_w || chess_color == cc_b);
			king_squares[chess_color]= square;
		}
	int Get_King_Square(int chess_color) const
		{
			assert (chess_color == cc_w || chess_color == cc_b);
			return king_squares[chess_color];
		}

	void Reset(); 
	/* Reset to the initial position.  (and legal mode)
	 */

	void Empty(); 
	/* Don't change MOVED.  Enter illegal mode. 
	 */

	void operator << (const Move &); 
	/* Play a move      */
	void operator >> (const Move &); 
	/* Take back a move */

	void Light_Play(const Move &);
	void Light_Take_Back(const Move &);
	/* These two functions play/take back a move, but ONLY change
	 * the pieces.  They are used by LEGAL.  
	 */

	int Square_Attacked(int square, 
			    int color,
			    bool multiple= false) const;
	/* Whether COLOR is attacked on SQUARE.  The return value is
	 * the number of times the square is attacked.  If MULTIPLE is
	 * FALSE, at most one attack is reported. 
	 *
	 * multiple=            FALSE                 TRUE
	 * Return value:  0:  not attacked      not attacked
	 *                1:  attacked          attacked once
	 *                2:  (never returned)  attacked at least twice
	 */

	int In_Check(bool multiple= false) const
		/* Whether the king of the side to move is in check.
		 * The return value is the same as that from
		 * Square_Attacked(). 
		 */
		{
			assert (legal);

			if (!multiple)
				return in_check ? 1 : 0;
			else
				return Square_Attacked(Get_King_Square(to_move), to_move, multiple);
		}

	bool Get_Legal() const
		{
			return legal;
		}
	void Update_Material();
	/* Must be in illegal mode:  MATERIAL is adjusted to
	 * correspond to the pieces. 
	 */
	bool Set_Legal();
	/* Enter legal mode.  Return TRUE when
	 * position is legal, or set the error. 
	 *
	 * MATERIAL must be correct.  (Call Update_Material() just
	 * before calling Set_Legal(), or build up MATERIAL while
	 * adding pieces.)   
	 *
	 * KING_SQUARES doesn't need to be correct. 
	 */

	void Remove_Piece(int square)
		/* While in illegal mode.  Don't update MATERIAL. 
		 */
		{
			assert (!legal);
			assert (Is_Inside_Square(square));
			pieces.pieces[square]= Piece(false);
		}
	int Get_To_Move() const
		{
			return to_move; 
		}
	void Transfer_Piece(int from, int to);
	void Change_To_Move(); 
	int Get_EP_Square() const
		{
			return ep_square; 
		}
	int &Get_EP_Square()
		{
			return ep_square; 
		}
	void Insert_Piece(int square, Piece piece); 
	/* While not in legal mode, but maintain MATERIAL. 
	 */
	int Get_Previous_Move_Count() const
		{
			assert ((previous_move_count + to_move) % 2 == 0); 
			return previous_move_count;
		}
	void Set_Previous_Move_Count(int new_previous_move_count)
		{
			assert (new_previous_move_count >= 0);
			assert ((new_previous_move_count + to_move) % 2 == 0); 
			previous_move_count= new_previous_move_count;
		}
	int Get_Halfmove_Clock() const
		{
			return halfmove_clock; 
		}
	int Get_Material(int cc, int pt) const
		{
			return material(cc, pt); 
		}
	Material &Get_Material()
		{
			assert (! legal); 
			return material; 
		}

	void Set_To_Move(int new_to_move)
		{
			if (new_to_move == to_move)
				return;
			Change_To_Move(); 
		}
	const Material &Get_Material_Const() const
		{
			return material; 
		}
	void Set_Halfmove_Clock(int new_halfmove_clock)
		{
			halfmove_clock= new_halfmove_clock; 
		}
	void Set_Illegal()
		{
			assert (legal);
			legal= false; 
		}
	bool operator == (const Position &p) const
		{
			return (to_move == p.to_move   &&
				ep_square == p.ep_square  &&
				!memcmp(pieces.pieces, p.pieces.pieces, sizeof (pieces))   &&
				!memcmp(moved, p.moved, sizeof(moved))       &&
				!memcmp(king_squares, p.king_squares, sizeof(king_squares))   &&
				previous_move_count == p.previous_move_count                  &&
				halfmove_clock == p.halfmove_clock                            &&
				material == p.material                                        &&
				in_check == p.in_check); 
		}
	bool operator != (const Position &p) const
		{
			return ! operator == (p); 
		}
	bool Would_Be_Legal(bool material_correct= false) const;
	/* In illegal mode.  Check whether the position would be
	 * legal.  If MATERIAL_CORRECT, MATERIAL is assumed to be
	 * correct.
	 */
	bool Attacked_From(int square);
	/* Whether the king to move is attacked from a piece on the
	 * file/rank/diagonal where SQUARE is on.  Only checks B/R/Q.  
	 * Note:  This function is used by LEGAL between Light_Play()
	 * and Light_Take_Back() and thus operates on the king that is
	 * NOT to move.  (but TO_MOVE is currently wrong) 
	 */
	void Invert();
	/* Invert black and white:  c4 becomes c5 etc...
	 * Works in any mode, as the legality isn't affected.
	 * MATERIAL is not affected.  Only in illegal mode,  
	 */
	void Mirror();
	/* right / left 
	 * MOVED is not changed. 
	 */
	
	static  bool Is_Move_Square(int square)
		/* Whether S is one of the squares a1, e1, h1, a8, e8 or h8. 
		 */ 
		{
			assert(square >= 0 && square < 120);
			int file= File(square), rank= Rank(square);
			return ((rank == sq_1 || rank == sq_8)
				&& (file == sq_a || file == sq_e || file == sq_h));
		}

	static int Square_Move_Index(int square)
/* S must be one of a1, e1, h1, a8, e8 or h8.  Return s the index of
 * this square. 
 */
		{
			assert(square == sq_a1 || square == sq_e1 || square == sq_h1
			       || square == sq_a8 || square == sq_e8 || square == sq_h8);
			if (square > 32)
				square -= 81;
			else
				square -= 20;
			return square / 3;
		}

private:

	class Piece_Array
	{
	public:
		Piece pieces[120];
		/* The pieces.  The index is the square number, as defined in
		 * SQUARE.  Squares outside the board are initialized by the
		 * constructor and are never changed, but can be read.
		 * The squares outside the board are set in the
		 * constructor of POSITION and are never changed. 
		 */

		void operator = (const Piece_Array &);
		/* Optimized to not copy the squares outside the
		 * board, which never change. 
		 */
		Piece &operator [] (int i) {return pieces[i];}
		Piece operator [] (int i) const {return pieces[i]; }
	};

	int to_move;
	/* who is to move */

	int ep_square;
	/* Whether the side to move can capture "en passant"; for
	 * instance e5.  0 if there is no "en passant" move.
	 */

	Piece_Array pieces;
//	Piece pieces[120];

	int moved[6]; 
	/* By index:  a1, e1, h1, a8, e8, h8 
	 *
	 * For each of these squares, the number of times a piece has
	 * moved on it.  If 0, the square contains the rook or king
	 * from the initial position.   
	 *	   
	 * When editing a position, moved[e1] and moved[e8] should
	 * always be 0, so there is:
	 *     wO-O   <=> moved[0] == 0
	 *     wO-O-O <=> moved[2] == 0
	 *     bO-O   <=> moved[3] == 0
	 *     bO-O-O <=> moved[5] == 0
	 */

	int king_squares[2];
	/* Position of kings. 
	 */

	int previous_move_count; 
	/* Number of halfmoves since beginning of game.  Even if white
	 * is to move, else odd. 
	 */

	int halfmove_clock; 

	Material material;
	/* Piece count.  In sync with PIECES. 
	 */
	bool legal;
	/* Whether position is legal.  Set to FALSE in Empty(). 
	 */
	bool in_check;
	/* Whether the king to move is in check.  Only in legal mode. 
	 */

	void Init_Outside();
	/* Set the squares outside the chess board to the appropriate
	 * value.  Called once in every constructor. 
	 */ 
	void Set_Material(Material &);
	bool Attacks(int square);
	/* Wether the king to move is in check by the piece on
	 * SQUARE.  There is always a piece of the color to move on
	 * square. 
	 */
};

#endif /* ! POSITION_INCLUDED */
