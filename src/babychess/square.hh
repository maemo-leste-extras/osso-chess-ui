#ifndef SQUARE_INCLUDED
#define SQUARE_INCLUDED

/* A square on a chess board.  "int" is used most of the time, but
 * 8 bit are enough. 
 */

#include "string.hh"
#include "chess_color.hh"
#include "inline.hh"

/*  
 *        A    B    C    D    E    F    G    H
 *    ||---------------------------------------||
 *  8 || 91 | 92 | 93 | 94 | 95 | 96 | 97 | 98 || 8
 *    ||---------------------------------------||
 *  7 || 81 | 82 | 83 | 84 | 85 | 86 | 87 | 88 || 7
 *    ||---------------------------------------||
 *  6 || 71 | 72 | 73 | 74 | 75 | 76 | 77 | 78 || 6
 *    ||---------------------------------------||
 *  5 || 61 | 62 | 63 | 64 | 65 | 66 | 67 | 68 || 5
 *    ||---------------------------------------||
 *  4 || 51 | 52 | 53 | 54 | 55 | 56 | 57 | 58 || 4
 *    ||---------------------------------------||
 *  3 || 41 | 42 | 43 | 44 | 45 | 46 | 47 | 48 || 3
 *    ||---------------------------------------||
 *  2 || 31 | 32 | 33 | 34 | 35 | 36 | 37 | 38 || 2
 *    ||---------------------------------------||
 *  1 || 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 || 1
 *    ||---------------------------------------||
 *        A    B    C    D    E    F    G    H
 */

/* FILE and RANK:  always zero-based:  a=0, h=7, "1"=0, "8"= 7. 
 */

enum {sq_a1= 21, sq_b1, sq_c1, sq_d1, sq_e1, sq_f1, sq_g1, sq_h1, 
      sq_a8= 91, sq_b8, sq_c8, sq_d8, sq_e8, sq_f8, sq_g8, sq_h8, 

      sq_a= 0, sq_b, sq_c, sq_d, sq_e, sq_f, sq_g, sq_h, 
      sq_1= 0, sq_2, sq_3, sq_4, sq_5, sq_6, sq_7, sq_8};

Inline void Square_Turn(int &square)
	/* The same square when the board is turned. */
{
	square= 119 - square;
}

Inline int File(int square) 
	/* the file of the square */
{
	int file= square % 10 - 1; 
	assert (file >= 0 && file < 8);
	return file;
}

Inline int Rank(int square) 
	/* the rank of the square */
{
	int result= square / 10 - 2;
	assert (result >= 0 && result < 8);
	return result;
}

Inline bool Is_Inside_Square(int square)
	/* Whether the square is one of 64 chess board squares. 
	 */
{
	return square >= 21 && square < 99 && square % 10 > 0 && square % 10 < 9; 
}

Inline int Make_Invalid_Square(int file, int rank)
     /* May be invalid.  */
{
	return 10 * rank + file + 21;
}

Inline int Make_Square(int file, 
		       int rank)
{
	//assert (file >= 0 && file < 8 && rank >= 0 && rank < 8);
	if (file<0) file = 0;
	if (file>7) file = 7;
	if (rank<0) rank = 0;
	if (rank>7) rank = 7;

	return Make_Invalid_Square(file, rank);
}

Inline int Square_Color(int s)
	/* S must be on the chess board */
{
	assert (Is_Inside_Square(s));
	return ((s / 10 + s) % 2 == 0) ? cc_w : cc_b;
}

Inline int Square_Invert(int square)
	/* invert black/white */
{
	return Make_Square(File(square),
			   7 - Rank(square));
}

Inline int Square_Mirror(int square)
{
	return Make_Square(7 - File(square),
			   Rank(square));
}

#endif /* ! SQUARE_INCLUDED */
