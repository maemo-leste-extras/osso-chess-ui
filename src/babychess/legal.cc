#include "legal.hh"

#include "moves.hh"
//#include "dbg.hh"
#include <libosso.h>

static bool legal_just_check;

static bool legal_strict_legal;
/* Whether checking for only legal moves. 
 */

static Pointer <Position> legal_position;
static Pointer <Vector <Move> > legal_moves;

static int legal_move_index;
/* Index where the next move will be written in GEN_LEG_MOVES. 
 */

static int legal_square;
/* Current square. 
 */

static int legal_to_move;
/* Side to move.
 */

static int legal_ep_poss;
/* Of analized position. 
 */

int Legal::flags;

bool Legal::Add_Move(int to, 
		     Piece captured_piece,
		     Piece promotion, 
		     int ep_capture, 
		     Move::Castling castling)
{
//	Dbg_("Add_Move (%d, %d)", legal_square, to);

	Move &move= legal_moves()[legal_move_index];
	move.from= legal_square;
	move.to= to;
	move.captured_piece= captured_piece;
	move.ep_poss_square= legal_ep_poss;
	move.promotion= promotion;
	move.ep_square= ep_capture;
	move.castling= castling;
	move.piece= legal_position()/*.Get_Piece*/[legal_square];
	move.halfmove_clock= legal_position().Get_Halfmove_Clock();
	move.check= legal_position().In_Check();

	bool append= true;
	/* Wether the move must be appended.  Set to FALSE
	 * when it is not legal and only legal moves are
	 * generated.  
	 */
	if (legal_strict_legal)
	{
		bool in_check= legal_position().In_Check();
		legal_position().Light_Play(move);
		int cc= legal_position().Get_To_Move();

//		Dbg_("\tin_check= %d, move.piece.Get_Type()= %d", in_check, 
//		    move.piece.Get_Type());

		if (in_check || (move.piece.Get_Type() == pt_K))
		{
			/* KING_SQUARE is not updated in the position. 
			 */
			int king_square= legal_position().Get_King_Square(cc);
			if (move.piece.Get_Type() == pt_K)
				king_square= to;
			append= ! legal_position().Square_Attacked
				(king_square, cc);
//			Dbg_("\tdirect => append= %d", append);
		}
		else /* King was not in check and hasn't moved.  Check
		      *	if the move would put the king in check */
		{
			bool a= legal_position().Attacked_From(move.from);
//			Dbg_Int_(a);
			append= ! a;
//			Dbg_("\tindirect => append= %d", append);
		}

		legal_position().Light_Take_Back(move);
	}
	
	if (append)
	{
		++ legal_move_index; 
		if (legal_just_check)
			return true;
		else
			return false;
	}
	else
		return false;
}

bool Legal::Pawn(Piece from_piece)
{
	if (from_piece.Get_Color() == cc_w)
		/* white pawn */
	{
		if (Rank(legal_square) == sq_7)
			/* white pawn on 7th rank */
		{
			int osize= legal_move_index; 
			if (! legal_position()/*.Get_Piece*/[legal_square + 10].Is())
			{
				for (int pt= pt_Q;  pt >= pt_N;  --pt)
					if (Add_Move
					    (legal_square + 10,
					     Piece(false),
					     Piece(cc_w, pt), 0))
						return true;
			}
			if (legal_position()/*.Get_Piece*/[legal_square + 9]
			    .Is_Piece_Of_Color(cc_b))
			{
				for (int pt= pt_Q;  pt >= pt_N;  -- pt)
					if (Add_Move
					    (legal_square + 9,
					     legal_position()/*.Get_Piece*/[legal_square + 9],
					     Piece(cc_w, pt), 0))
						return true;
			}
			if (legal_position()/*.Get_Piece*/[legal_square + 11] 
			    .Is_Piece_Of_Color(cc_b))
			{
				for (int pt= pt_Q;  pt >= pt_N;  --pt)
					if (Add_Move
					    (legal_square +11,
					     legal_position()/*.Get_Piece*/[legal_square + 11],
					     Piece(cc_w, pt), 0))
						return true;
			}
			if (osize != legal_move_index)
				flags |= gn_promotion;
		}
		else
		{
			if (! legal_position()/*.Get_Piece*/[legal_square + 10].Is())
				if (Add_Move
				    (legal_square + 10,
				     Piece(false), 0, 0))
					return true;
			if (Rank(legal_square) == sq_2   &&
			    legal_position()/*.Get_Piece*/[legal_square + 10] == Piece(false) &&
			    legal_position()/*.Get_Piece*/[legal_square + 20] == Piece(false))
				if (Add_Move
				    (legal_square + 20,
				     Piece(false), 0, 0))
					return true;
			if (legal_position()/*.Get_Piece*/[legal_square + 9]
			    .Is_Piece_Of_Color(cc_b))
				if (Add_Move
				    (legal_square + 9,
				     legal_position()/*.Get_Piece*/[legal_square + 9], 0, 0))
					return true;
			if (legal_position()/*.Get_Piece*/[legal_square + 11]
			    .Is_Piece_Of_Color(cc_b))
				if (Add_Move
				    (legal_square + 11,
				     legal_position()/*.Get_Piece*/[legal_square+11], 0, 0))
					return true;
			if (Rank(legal_square) == sq_5)
			{
				if (legal_position().Get_EP_Square() == legal_square - 1)
					if (Add_Move
					    (legal_square + 9,
					     Piece(cc_b, pt_P), 0,
					     legal_square - 1))
						return true;
				if (legal_position().Get_EP_Square() == legal_square + 1)
					if (Add_Move
					    (legal_square + 11,
					     Piece(cc_b, pt_P), 0,
					     legal_square + 1))
						return true;
			}
		}
	}
	else /* black pawn */
	{
		if (Rank(legal_square) == sq_2)
			/* black pawn on second rank */
		{
			int osize= legal_move_index; 
			if (! legal_position()/*.Get_Piece*/[legal_square - 10].Is())
			{
				for (int pt= pt_Q;  pt >= pt_N;  --pt)
					if (Add_Move
					    (legal_square - 10,
					     Piece(false),
					     Piece(cc_b, pt), 0))
						return true;
			}
			if (legal_position()/*.Get_Piece*/[legal_square - 9]
			    .Is_Piece_Of_Color(cc_w))
			{
				for (int pt= pt_Q;  pt >= pt_N;  --pt)
					if (Add_Move
					    (legal_square - 9,
					     legal_position()/*.Get_Piece*/[legal_square - 9],
					     Piece(cc_b, pt), 0))
						return true;
			}
			if (legal_position()/*.Get_Piece*/[legal_square - 11]
			    .Is_Piece_Of_Color(cc_w))
			{
				for (int pt= pt_Q;  pt >= pt_N;  --pt)
					if (Add_Move
					    (legal_square - 11,
					     legal_position()/*.Get_Piece*/[legal_square - 11],
					     Piece(cc_b, pt), 0))
						return true;
			}
			if (osize != legal_move_index)
				flags |= gn_promotion;
		}
		else /* other black pawn */
		{
			if (! legal_position()/*.Get_Piece*/[legal_square - 10].Is())
				if (Add_Move
				    (legal_square - 10,
				     Piece(false), 0, 0))
					return true;
			if (Rank(legal_square) == sq_7
			    && (legal_position()/*.Get_Piece*/[legal_square - 10]
				== Piece(false))
			    && (legal_position()/*.Get_Piece*/[legal_square - 20]
				== Piece(false)))
				if (Add_Move
				    (legal_square - 20,
				     Piece(false), 0, 0))
					return true;
			if (legal_position()/*.Get_Piece*/[legal_square - 11]
			    .Is_Piece_Of_Color(cc_w))
				if (Add_Move
				    (legal_square-11,
				     legal_position()/*.Get_Piece*/[legal_square - 11], 0, 0))
					return true;
			if (legal_position()/*.Get_Piece*/[legal_square - 9]
			    .Is_Piece_Of_Color(cc_w))
				if (Add_Move
				    (legal_square - 9,
				     legal_position()/*.Get_Piece*/[legal_square - 9], 0, 0))
					return true;
			if (Rank(legal_square) == sq_4)
			{
				if (legal_position().Get_EP_Square() == legal_square - 1)
					if (Add_Move
					    (legal_square - 11,
					     Piece(cc_w, pt_P), 0,
					     legal_square - 1))
						return true;
				if (legal_position().Get_EP_Square() == legal_square + 1)
					if (Add_Move
					    (legal_square - 9,
					     Piece(cc_w, pt_P), 0,
					     legal_square + 1))
						return true;
			}
		}
	}
	
	return false;
}

void Legal::Generate(Position &position,
		     Vector <Move> &moves,
		     bool strict_legal, 
		     bool just_check)
{
//	Dbg_Function_();

	legal_just_check= just_check;
	legal_strict_legal= strict_legal;
	legal_moves= moves;
	legal_position= position;

	legal_moves().Set(just_check ? 1 : max_leg);
	legal_move_index= 0;



	legal_to_move= position.Get_To_Move();
	legal_ep_poss= position.Get_EP_Square();
	flags= 0;
  
	for (int rank= sq_1;  rank <= sq_8;  ++ rank)
	{
		for (int file= sq_a;  file <= sq_h;  ++ file)
		{
			legal_square= Make_Square(file, rank);
			Piece from_piece= position/*.Get_Piece*/[legal_square];
			if (from_piece.Is_Piece_Of_Color(legal_to_move))
			{
				/*osso_log(LOG_DEBUG,"Legal: type: %d ",
					from_piece.Get_Type());
					*/
				switch (from_piece.Get_Type())
				{
				case pt_P: /* pawn */
					if (Legal::Pawn(from_piece))
						goto end;
					break;
				case pt_N: /* knight */
					for (int i= 8; i < 16;  ++i)
					{
						int f= legal_square + moves_offset[i]; 
						Piece g= position/*.Get_Piece*/[f];
						if (g.Can_Color_Move_To(legal_to_move))
							if (Add_Move(f, g, 0, 0))
								goto end;
					}
					break;

				case pt_B:  /* long move */
				case pt_R:     
				case pt_Q:
				{
					int pt= position/*.Get_Piece*/[legal_square].Get_Type(); 
					int j0= moves_range[0][pt - 2];
					int j1= moves_range[1][position[legal_square].Get_Type() - 2]; 
					assert (j0 < j1);
					for (int i= j0;  i < j1;  ++i) 
					{
						int s= legal_square + moves_offset[i];
						while (position[s].Is_Empty())
						{
							if (Add_Move
							    (s, Piece(false), 0, 0))
								goto end;
							s += moves_offset[i];
						}
						Piece piece= position/*.Get_Piece*/[s];
						if (piece.Is_Piece_Of_Color(1^legal_to_move))
							if (Add_Move(s, piece, 0 , 0))
								goto end;
					}
					break;
				}

				case pt_K: /* king */
		  
					for (int i= 0; i < 8; ++i)
					{
						int s= legal_square + moves_offset[i];
						Piece p= position/*.Get_Piece*/[s];
						if (p.Can_Color_Move_To(legal_to_move))
							if (Add_Move(s, p, 0, 0))
								goto end;
					}
					if (legal_square == sq_e1 && !position.Get_Moved(1))
						/* white castling */
					{
						if (position/*.Get_Piece*/[sq_f1] == Piece(false)
						    && position/*.Get_Piece*/[sq_g1] == Piece(false)
						    && !position.Square_Attacked(sq_f1, cc_w)
						    && !position.Get_Moved(2)
						    && !position.Square_Attacked(sq_e1, cc_w))
							if (Add_Move(sq_g1, Piece(false), 0, 0,  
									   Move::w_O_O))
								goto end;
						if (position/*.Get_Piece*/[sq_d1] == Piece(false)
						    && position/*.Get_Piece*/[sq_c1] == Piece(false)
						    && position/*.Get_Piece*/[sq_b1] == Piece(false)
						    && !position.Square_Attacked(sq_d1, cc_w)
						    && !position.Get_Moved(0)
						    && !position.Square_Attacked(sq_e1, cc_w))
							if (Add_Move(sq_c1, Piece(false), 0, 0, 
									     Move::w_O_O_O))
								goto end;
					}
					else if (legal_square == sq_e8 && !position.Get_Moved(4))
							/* black castling */
						{
							if (position/*.Get_Piece*/[sq_f8] == Piece(false) &&
							    position/*.Get_Piece*/[sq_g8] == Piece(false) &&
							    ! position.Square_Attacked(sq_f8, cc_b) &&
							    ! position.Get_Moved(5)
							    && !position.Square_Attacked(sq_e8, cc_b))
								if (Add_Move
								    (sq_g8, Piece(false),
								     0, 0, Move::b_O_O))
									goto end;

							if (position/*.Get_Piece*/[sq_d8] == Piece(false)
							    && position/*.Get_Piece*/[sq_c8] == Piece(false)
							    && position/*.Get_Piece*/[sq_b8] == Piece(false)
							    && !position.Square_Attacked(sq_d8, cc_b)
							    && !position.Get_Moved(3)
							    && !position.Square_Attacked(sq_e8, cc_b))
								if (Add_Move
								    (sq_c8, Piece(false), 0, 0,
								     Move::b_O_O_O))
									goto end;
						}
					break;

				default:
					assert(0);
				}
			}
		}
	}

 end:

	legal_moves()= legal_move_index; 

	legal_position= NULL;
	legal_moves= NULL;
}
