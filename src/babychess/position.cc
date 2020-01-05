#include "position.hh"

#include "moves.hh"
#include <glib.h>

#include <libintl.h>
#include <libosso.h>

void Position::Init_Outside()
{
	/* above & below */
	for (int i= 0;  i < 2;  ++i)
	{
		for (int j= i * 100;  j < i * 100 + 20; ++j)
			pieces[j]= pi_outside;
	}

	/* left & right */
	for (int i= 20; i < 100;  i += 10)
	{
		pieces[i]= pi_outside; 
		pieces[i + 9]= pi_outside; 
	}
}

void
Position::Reset()
{
	/* enter legal mode */ 
	legal= true;

	/* empty squares */
	for (int rank= sq_3;  rank <= sq_6;  ++rank) {
		for (int i=0; i<8; i++)
			pieces.pieces[Make_Square(sq_a, rank)+i].Set_Empty();
	}

	/* white pawns */
	for (int i= 31; i<39; ++i)
		pieces[i]= pi_wP; 

	/* black pawns */
	for (int i= 81; i<89; ++i)
		pieces[i]= pi_bP; 

	/* First rows */
	int first_row[]= {pt_R, pt_N, pt_B, pt_Q, pt_K, pt_B, pt_N, pt_R};

	for (int file= sq_a;  file <= sq_h;  ++file)
	{
		pieces[Make_Square(file, sq_1)]= Piece(cc_w, first_row[file - sq_a]);
		pieces[Make_Square(file, sq_8)]= Piece(cc_b, first_row[file - sq_a]);
	}

	/* Material
	 */
	for (int cc= 0;  cc < 2;  ++cc)
	{
		material(cc, pt_P)= 8;
		material(cc, pt_N)= 2;
		material(cc, pt_B)= 2;
		material(cc, pt_R)= 2;
		material(cc, pt_Q)= 1;
		material(cc, pt_K)= 1;
	}

	/* Other 
	 */
	for (int i= 0;  i < 6;  ++i)
		moved[i]= 0;
	to_move= cc_w;
	ep_square= 0;
	king_squares[cc_w]= sq_e1;
	king_squares[cc_b]= sq_e8;
	previous_move_count= 0;
	halfmove_clock= 0;
	in_check= false;
}

void Position::Empty()
{
	legal= false;

	/** pieces */

	/* make sure an empty square has the value 32 */ 
	assert (sizeof(Piece) >= 1);
	Piece p(false);
	assert ((*(char*)&p) == 32);
	for (int rank= sq_1;  rank <= sq_8;  ++rank)
	{
		//memset(pieces.pieces + (rank*10 + 21), 32, 8);
		for (int i=0; i<8; i++)
			pieces.pieces[rank*10 + 21 + i].Set_Empty();
	}

	/** rest */
	material.Clear();
	to_move= cc_w;
	previous_move_count= 0; /* == to_move */ 
	halfmove_clock= 0;
}

void
Position::operator << (const Move &move)
{
	assert (legal); 
	assert (pieces[move.from].Is());
	assert (move.ep_square != 0 || pieces[move.to] == move.captured_piece);
	assert (move.to > 0 && move.to < 120);

	/* Move the piece */

	pieces[move.to]= pieces[move.from];
	pieces[move.from]= pi_empty;

	/* Castling */

	if (move.castling)
	{
		switch (move.castling)
		{
		case Move::w_O_O:
			pieces[sq_h1]= Piece(false);
			pieces[sq_f1]= Piece(cc_w, pt_R);
			break;
  
		case Move::w_O_O_O:
			pieces[sq_a1]= Piece(false);
			pieces[sq_d1]= Piece(cc_w, pt_R);
			break;
  
		case Move::b_O_O:
			pieces[sq_h8]= Piece(false);
			pieces[sq_f8]= Piece(cc_b, pt_R);
			break;

		case Move::b_O_O_O:
			pieces[sq_a8]= Piece(false);
			pieces[sq_d8]= Piece(cc_b, pt_R);
			break;

		default:
			assert(0);
		}
	}

	/* Change side to move */
	
	to_move ^= 1;

	/* Update MOVED */ 

	if (Is_Move_Square(move.from))
		++ moved[Square_Move_Index(move.from)];

	if (Is_Move_Square(move.to))
		++ moved[Square_Move_Index(move.to)];

	/* MATERIAL & HALFMOVE_CLOCK */

	if (move.captured_piece.Is())
	{
		-- material(to_move, move.captured_piece.Get_Type());
		halfmove_clock= 0;
	}
	else
		++ halfmove_clock;

	if (pieces[move.to].Get_Type() == pt_P)
		halfmove_clock= 0;

	/* Promotion */ 

	if (move.promotion.Is())
	{
		pieces[move.to]= move.promotion;
		-- material(1^to_move, pt_P);
		++ material(1^to_move, move.promotion.Get_Type());
	}

	/* En passant capture */

	else if (move.ep_square != 0)
		pieces[move.ep_square]= Piece(false);

	/* New en passant square for double pawn advances */ 

	if (pieces[move.to] == Piece(cc_w, pt_P) && 
	    Rank(move.from) == sq_2       &&
	    Rank(move.to) == sq_4)
		ep_square= move.to;
	else if (pieces[move.to] == Piece(cc_b, pt_P) &&
		 Rank(move.from) == sq_7       &&
		 Rank(move.to) == sq_5)
		ep_square= move.to;
	else
		ep_square= 0;

	/* king square */

	if (pieces[move.to].Is_Piece_Of_Type(pt_K))
	{
		king_squares[pieces[move.to].Get_Color()]= move.to; 
	}

	/* in_check */
	in_check= false;
	if (Attacks(move.to)) /* may be a promoted piece */
		in_check= true;
	else if (Attacked_From(move.from))
		in_check= true;
	else if (move.ep_square && Attacked_From(move.ep_square))
		in_check= true;
	else if (move.castling)
	{
		static const int rook_squares[4]= {sq_f1, sq_d1, sq_f8, sq_d8};
		if (Attacks(rook_squares[move.castling-4]))
			in_check= true;
	}

	/* previous moves */
	++ previous_move_count;

	/* material check */ 
	assert (material.Check()); 
}

void Position::Light_Play(const Move &move)
{
	/* Content mostly copied from operator << (). 
	 */

	pieces[move.to]= pieces[move.from];
	pieces[move.from]= pi_empty;
	if (move.castling)
	{
		switch (move.castling)
		{
		case Move::w_O_O:
			pieces[sq_h1]= Piece(false);
			pieces[sq_f1]= Piece(cc_w, pt_R);
			break;
  
		case Move::w_O_O_O:
			pieces[sq_a1]= Piece(false);
			pieces[sq_d1]= Piece(cc_w, pt_R);
			break;
  
		case Move::b_O_O:
			pieces[sq_h8]= Piece(false);
			pieces[sq_f8]= Piece(cc_b, pt_R);
			break;

		case Move::b_O_O_O:
			pieces[sq_a8]= Piece(false);
			pieces[sq_d8]= Piece(cc_b, pt_R);
			break;

		default:
			assert(0);
		}
	}
	else if (move.ep_square != 0)
		pieces[move.ep_square]= Piece(false);
	else if (move.promotion.Is())
	{
		pieces[move.to]= move.promotion;
	}
}

void Position::operator >> (const Move &move)
{
	assert(legal);
	assert(pieces[move.to].Is());

	/* pieces */ 

	pieces[move.from]= pieces[move.to];
	pieces[move.to]= move.captured_piece;

	/* TO_MOVE */ 

	to_move ^= 1;

	/* MOVED */ 

	if (Is_Move_Square(move.from))
		-- moved[Square_Move_Index(move.from)];
	if (Is_Move_Square(move.to))
		-- moved[Square_Move_Index(move.to)];

	/* old EP_SQUARE */ 

	ep_square= move.ep_poss_square;

	/* MATERIAL */

	if (move.captured_piece.Is())
	{
		++ material(1^to_move, move.captured_piece.Get_Type());
	}

	/* halfmove clock */

	halfmove_clock= move.halfmove_clock; 
	assert (halfmove_clock >= 0);
	
	/* castling */ 

	if (move.castling != 0)
	{
		switch (move.castling)
		{
		case Move::w_O_O:
			pieces[sq_f1]= Piece(false);
			pieces[sq_h1]= Piece(cc_w, pt_R);
			break;

		case Move::w_O_O_O:
			pieces[sq_d1]= Piece(false);
			pieces[sq_a1]= Piece(cc_w, pt_R);
			break;

		case Move::b_O_O:
			pieces[sq_f8]= Piece(false);
			pieces[sq_h8]= Piece(cc_b, pt_R);
			break;

		case Move::b_O_O_O:
			pieces[sq_d8]= Piece(false);
			pieces[sq_a8]= Piece(cc_b, pt_R);
			break;

		default:
			assert (0);
		}
	}

	/* Take back en passant move */

	else if (move.ep_square != 0)
	{
		assert(ep_square == move.ep_square);
		pieces[move.ep_square]= move.captured_piece;
		pieces[move.to]= Piece(false);
	}

	/* Take back promotion */ 
	
	else if (move.promotion.Is())
	{
		pieces[move.from]= Piece(to_move, pt_P);
		++ material(to_move, pt_P);
		-- material(to_move, move.promotion.Get_Type());
	}

	/* king square */ 

	if (pieces[move.from].Is_Piece_Of_Type(pt_K))
		king_squares[pieces[move.from].Get_Color()]= move.from;

	/* previous moves */ 

	assert (previous_move_count > 0);
	--previous_move_count;

	assert (material.Check());

	/* in_check */
	in_check= move.check; 
}

void Position::Light_Take_Back(const Move &move)
{
	/* Content mostly copied from operator >> (). 
	 */

	pieces[move.from]= pieces[move.to];
	pieces[move.to]= move.captured_piece;
	if (move.castling != 0)
	{
		switch (move.castling)
		{
		case Move::w_O_O:
			pieces[sq_f1]= Piece(false);
			pieces[sq_h1]= Piece(cc_w, pt_R);
			break;

		case Move::w_O_O_O:
			pieces[sq_d1]= Piece(false);
			pieces[sq_a1]= Piece(cc_w, pt_R);
			break;

		case Move::b_O_O:
			pieces[sq_f8]= Piece(false);
			pieces[sq_h8]= Piece(cc_b, pt_R);
			break;

		case Move::b_O_O_O:
			pieces[sq_d8]= Piece(false);
			pieces[sq_a8]= Piece(cc_b, pt_R);
			break;

		default:
			assert (0);
		}
	}

	/* Take back en passant move */

	else if (move.ep_square != 0)
	{
		assert(ep_square == move.ep_square);
		pieces[move.ep_square]= move.captured_piece;
		pieces[move.to]= Piece(false);
	}

	/* Take back promotion */ 
	
	else if (move.promotion.Is())
	{
		pieces[move.from]= Piece(to_move, pt_P);
	}

}

int Position::Square_Attacked(int square, 
			  int color,
			  bool multiple) const 
{
//	Dbg_Function_(); 

	int max= multiple ? 2 : 1;
	int result= 0;

#define FOUND    ++result;  if (result == max)  return result; 

	/* king */
	for (int offs= 0;  offs < 8;  ++ offs)
		if (pieces[square + moves_offset[offs]] == Piece(1^color, pt_K))  {FOUND}

	/* pawn */
	if (color == cc_b)
	{
		if (pieces[square -  9] == Piece(cc_w, pt_P))  {FOUND}
		if (pieces[square - 11] == Piece(cc_w, pt_P))  {FOUND}
	}
	else
	{
		if (pieces[square +  9] == Piece(cc_b, pt_P)) {FOUND}
		if (pieces[square + 11] == Piece(cc_b, pt_P)) {FOUND}
	}

	/* files and ranks */
	for (int offs= 0;  offs < 4;  ++ offs)
	{
		int i= 1;

		while (pieces[square + i * moves_offset[offs]].Is_Empty())
			++i;

		if (pieces[square + i * moves_offset[offs]] == Piece(1^color, pt_Q)   ||
		    pieces[square + i * moves_offset[offs]] == Piece(1^color, pt_R))
		{FOUND}
	}

	/* diagonals */
	for (int offs= 4;  offs < 8;  ++ offs)
	{
		int i= 1;
      
		while (pieces[square + i * moves_offset[offs]].Is_Empty())
			++i;

		if (pieces[square + i * moves_offset[offs]] == Piece(1^color, pt_Q) ||
		    pieces[square + i * moves_offset[offs]] == Piece(1^color, pt_B))
		{FOUND}
	}

	/* knight */
	for (int offs= 8;  offs < 16;  ++ offs)
		if (pieces[square + moves_offset[offs]] == Piece(1^color, pt_N)) 
		{FOUND}

#undef FOUND

	return result; 
}

void Position::Update_Material()
{
	assert (!legal);

	Set_Material(material);
}

void Position::Set_Material(Material &material)
{
	for (int cc= 0;  cc < 2;  ++cc)
		for (int pt= 0;  pt < 6;  ++pt)
			material(cc, pt)= 0;

	for (int file= sq_a;  file <= sq_h;  ++file)
		for (int rank= sq_1;  rank <= sq_8;  ++rank)
		{
			Piece piece= pieces[Make_Square(file, rank)];
			if (piece.Is())
				++ material(piece.Get_Color(), piece.Get_Type());
		}
}

void Position::Transfer_Piece(int from, int to)
{
	assert (!legal);
	assert (Is_Inside_Square(from));
	assert (Is_Inside_Square(to));

	Piece to_p= pieces[to];
	if (to_p.Is())
		-- material(
			to_p.Get_Color(),
			to_p.Get_Type());
	pieces[to]= pieces[from];
	pieces[from]= Piece(false);
	if (pieces[to].Get_Type() == pt_K)
		king_squares[pieces[to].Get_Color()]= to; 
}

void Position::Change_To_Move()
{
	assert (! legal);
	if (to_move != cc_w)
	{
		to_move= cc_w;
		-- previous_move_count;
	}
	else
	{
		to_move= cc_b;
		++ previous_move_count;
	}
}

void Position::Insert_Piece(int square, Piece piece)
{
	assert (!legal);
	assert (Is_Inside_Square(square));
	assert (piece.Is());
	assert (! pieces[square].Is());

	pieces[square]= piece;
	++ material(
		piece.Get_Color(),
		piece.Get_Type());
	if (piece.Get_Type() == pt_K)
		king_squares[piece.Get_Color()]= square;
}

bool Position::Set_Legal()
{
	assert (!legal);

	/* check material */
	Material real_material;
	Set_Material(real_material);
	assert (material == real_material);
	assert (material.Check()); 

	/* no pawns on first row */
	for (int file= sq_a;  file <= sq_h;  ++file)
	{
		{
			Piece piece= pieces[Make_Square(file, sq_1)];
			if ((!piece.Is_Empty()) && piece.Get_Type() == pt_P)
			{
				return false;
			}
		}
		{
			Piece piece= pieces[Make_Square(file, sq_8)];
			if ((!piece.Is_Empty()) && piece.Get_Type() == pt_P)
			{
				return false;
			}
		}
	}

	/* no multiple kings */
	int count[2]= {0, 0};
	for (int file= sq_a;  file <= sq_h;  ++file)
		for (int rank= sq_1;  rank <= sq_8;  ++rank)
		{
			Piece piece= pieces[Make_Square(file, rank)];
			if (piece.Is_Piece_Of_Type(pt_K))
			{
				++ count[piece.Get_Color()]; 
				king_squares[piece.Get_Color()]= Make_Square(file, rank); 
			}
		}
	if (count[0] < 1)
	{
		return false;
	}
	if (count[0] > 1)
	{
		return false;
	}
	if (count[1] < 1)
	{
		return false;
	}
	if (count[1] > 1)
	{
		return false;
	}
	
	/* check king position */

	assert (pieces[Get_King_Square(cc_w)] == pi_wK);
	assert (pieces[Get_King_Square(cc_b)] == pi_bK);

	/* king in check */ 
	int square= Get_King_Square(!to_move);
	if (Square_Attacked(square, !to_move))
	{
		return false;
	}

	/* in_check */
	in_check= Square_Attacked(Get_King_Square(to_move), to_move); 

	/* no pinned pawn that can be captured e.p.  (That's illegal
	 * because the king would have been in check while the other
	 * side was to move.  LEGAL assumes that fact for an
	 * optimization.) 
	 */
	if (ep_square)
	{
		assert (pieces[ep_square] == Piece(1^to_move, pt_P));
		pieces[ep_square]= pi_empty;
		bool error= false;
		if (Attacked_From(ep_square))
		{
			error= true;
		}
		pieces[ep_square]= Piece(1^to_move, pt_P);
		if (error)  return false;
	}

	legal= true;
	return true;
}

bool Position::Attacked_From(int square)
{

	int king_square= king_squares[to_move];
	int king_file= File(king_square);
	int king_rank= Rank(king_square);
	int file= File(square);
	int rank= Rank(square);

	int step= 0;
	int piece_mask= 0;
	if (king_file == file)  /* same file */
	{
		step= rank-king_rank >= 0 ? 10 : -10;
		piece_mask= (1 << pt_R) | (1 << pt_Q);
	}
	else if (king_rank == rank) /* same rank */ 
	{
		step= file-king_file >= 0 ?  1 :  -1;
		piece_mask= (1 << pt_R) | (1 << pt_Q);
	}
	else if (king_file - file == king_rank - rank) /* a1-h8 type diagonal */
	{
		step= file-king_file >= 0 ? 11 : -11;
		piece_mask= (1 << pt_B) | (1 << pt_Q);
	}
	else if (file - king_file == king_rank - rank) /* a8-h1 type diagonal */
	{
		step= file-king_file >= 0 ?  -9 :  9;
		piece_mask= (1 << pt_B) | (1 << pt_Q);
	}

	if (step)
	{
		king_square += step;
		while (pieces[king_square].Is_Empty()) 
			king_square += step;

		if (pieces[king_square].Is()     &&
		    pieces[king_square].Get_Color() == 1^to_move   &&
		    ((1 << pieces[king_square].Get_Type()) & piece_mask))
			{
				return true;
			}
	}
	return false;
}

bool Position::Attacks(int square)
{
	assert (Is_Inside_Square(square));
	assert (pieces[square].Is_Piece_Of_Color(1^to_move));
	
	int pt= pieces[square].Get_Type();

	assert (pt >= 0 && pt <= pt_K);

	if (pt == pt_K)
		return false;  /* kings don't give check */ 

	/* pawn */ 
	if (pt == pt_P)
	{
		if (to_move == cc_b)
		{
			if (pieces[square+ 9] == pi_bK)
				return true;
			if (pieces[square+11] == pi_bK)
				return true;
			return false;
		}
		else
		{
			if (pieces[square- 9] == pi_wK)
				return true;
			if (pieces[square-11] == pi_wK)
				return true;
			return false;
		}
	}

	Piece king(to_move, pt_K);

	/* knight */ 
	if (pt == pt_N)
	{
		for (int i= 8;  i < 16;  ++i)
		{
			if (pieces[square + moves_offset[i]] == king)
				return true;
		}
		return false;
	}
	
	/* long moving pieces */
	int king_square= king_squares[to_move];
	int file= File(square), rank= Rank(square);
	int king_file= File(king_square), king_rank= Rank(king_square);

	int step= 0, piece_mask= 0;
	if (file == king_file)
	{
		step= king_rank-rank >= 0 ? 10 : -10;
		piece_mask= (1 << pt_R) | (1 << pt_Q);
	}
	else if (king_rank == rank) /* same rank */ 
	{
		step= king_file-file >= 0 ?  1 :  -1;
		piece_mask= (1 << pt_R) | (1 << pt_Q);
	}
	else if (king_file - file == king_rank - rank) /* a1-h8 type diagonal */
	{
		step= king_file-file >= 0 ? 11 : -11;
		piece_mask= (1 << pt_B) | (1 << pt_Q);
	}
	else if (file - king_file == king_rank - rank) /* a8-h1 type diagonal */
	{
		step= file-king_file >= 0 ?  9 :  -9;
		piece_mask= (1 << pt_B) | (1 << pt_Q);
	}

	if (!step)
		return false;

	if (! (piece_mask & (1 << pieces[square].Get_Type())))
		return false; 

	do
		square += step;
	while (pieces[square] == pi_empty);

	return king_square == square; 
}

bool Position::Would_Be_Legal(bool material_correct) const
{
	Position p(*this);

	if (!material_correct)
	{
		p.Update_Material();
	}

	return p.Set_Legal(); 
}

void Position::Invert()
{
	to_move ^= 1;
	if (ep_square)
		ep_square= Square_Invert(ep_square);

	for (int file= sq_a;  file <= sq_h;  ++file)
	{
		for (int rank= sq_1;  rank <= sq_4;  ++rank)
		{
			int square= Make_Square(file, rank);
			int osquare= Make_Square(file, 7-rank);
			Piece tmp= pieces[square];
			pieces[square]= pieces[osquare];
			pieces[osquare]= tmp;
			if (pieces[square].Is())
				pieces[square].Invert();
			if (pieces[osquare].Is())
				pieces[osquare].Invert();
		}
	}

	for (int i= 0;  i < 3;  ++i)
	{
		int tmp= moved[i];
		moved[i]= moved[i+3];
		moved[i+3]= tmp;
	}
	int tmp= king_squares[cc_w];
	king_squares[cc_w]= king_squares[cc_b];
	king_squares[cc_b]= tmp;
	if (to_move)
		++previous_move_count;
	else
		--previous_move_count; 
}

void Position::Mirror()
{
	assert (! legal);
	if (ep_square)
		ep_square= Square_Mirror(ep_square);

	for (int rank= sq_1;  rank <= sq_8;  ++rank)
	{
		for (int file= sq_a;  file <= sq_d;  ++file)
		{
			int square= Make_Square(file, rank);
			int osquare= Make_Square(7-file, rank);
			Piece tmp= pieces[square];
			pieces[square]= pieces[osquare];
			pieces[osquare]= tmp;
		}
	}

	king_squares[cc_w]= Square_Mirror(king_squares[cc_w]);
	king_squares[cc_b]= Square_Mirror(king_squares[cc_b]);
}

void Position::Piece_Array::operator = (const Piece_Array &array)
{
	for (int rank= sq_1;  rank <= sq_8;  ++rank) {
		for (int i=0; i<8; i++)
			pieces[Make_Square(sq_a, rank)+i] =
			  array.pieces[Make_Square(sq_a, rank)+i];
	}
}
