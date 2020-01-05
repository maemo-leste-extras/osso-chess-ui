#ifndef DRAW_BOARD_INCLUDED
#define DRAW_BOARD_INCLUDED

/* Functions that draw the chess board.  Used by BOARD and
 * BOARD_PREVIEW_CONTENT. 
 */

#include "piece.hh"
#include "canvas.hh"
#include "scaled_board_image.hh"
#include "board_settings.hh"

class Board_Colors
{
public:
	Board_Colors():  have_colors(false) {}
	void Make(const Board_Settings &);
	void Invalidate() {have_colors= false; }
	Canvas::Color operator () (int i, int dark) const
		{
			assert (have_colors);
			return colors[i][dark]; 
		}

private:

	bool have_colors; 
	Canvas::Color colors[c_count][2];
	/* Index 0 as in Settings::colors.  Set if HAVE_COLORS.  
	 * Second index:  dark color for the
	 * ring (only for colors that are used for circles)
	 */
};

void Read_Image();
Server_Image *Get_Image();

void Draw_Square(Canvas &,
		 Scaled_Board_Image &,
		 const Board_Colors &colors,
		 Point,
		 Piece,
		 int square_color,
		 int circle_color, /* 0 or one of the
				 * Boards_Settings::colors for a
				 * circle */
		 int flags,
		 int square,
		 bool selected);

void Draw_Rectangle(Canvas &canvas, 
		 Rect rect);
void Draw_Selection_Rectangle(Canvas &canvas, 
		 Rect rect);

#endif /* ! DRAW_BOARD_INCLUDED */ 

