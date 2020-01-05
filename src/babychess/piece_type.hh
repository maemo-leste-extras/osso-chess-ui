#ifndef PIECE_TYPE_INCLUDED
#define PIECE_TYPE_INCLUDED

/* The type of a chess piece. 
 */

enum 
{
	pt_P= 0, /* pawn   */
	pt_N,    /* knight */
	pt_B,    /* bishop */
	pt_R,    /* rook   */
	pt_Q,    /* queen  */
	pt_K,    /* king   */

	pt_count
};

//typedef int Piece_Type;

#endif /* ! PIECE_TYPE_INCLUDED */
