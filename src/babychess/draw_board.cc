#include "draw_board.hh"

#include "square.hh"
#include "format.hh"
#include "stream_file.hh"
#include "server_image.hh"
#include "alpha_server_image_stream.hh"
#include "server_image_stream.hh"
#include "settings.h"
#include "dbus.h"

#define BOARD_IMAGE "Background_board.jpg"
#define BOARD_IMAGE_FLIP_LETTERS "Background_board_flipletters.png"
#define BOARD_IMAGE_FLIP_NUMBERS "Background_board_flipnumbers.png"
#define SELECTION_IMAGE "selection.png"
#define LEGAL_MOVE_IMAGE "legal_move.png"
#define BOARD_IMAGE_LEFT 12
#define BOARD_IMAGE_TOP 5
Server_Image image;
Server_Image image_letters;
Server_Image image_numbers;
Alpha_Server_Image selection;
Alpha_Server_Image legal;

void Board_Colors::Make(const Board_Settings &settings)
{
	have_colors= true;

	for (int i= 0;  i < c_count;  ++i)
		colors[i][0]= Canvas::Get_Color(settings.colors[i]);

	/* colors that are used for circles */
	enum {count= 3};
	const int indices[count]= {c_last_move, c_highlight};
	for (int i= 0;  i < count;  ++i)
	{
		int index= indices[i];
		RGB color= settings.colors[index];
		color.r -= 50;  color.r = max(color.r, 0);
		color.g -= 50;  color.g = max(color.g, 0);
		color.b -= 50;  color.b = max(color.b, 0);
		colors[index][1]= Canvas::Get_Color(color);
	}
}

/* Read the board image */
void Read_Image()
{
	if ( !image.Is() )
		Read_From_File( PIXMAPSDIR "/" BOARD_IMAGE, image);
	if ( !image_numbers.Is() )
		Read_From_File( PIXMAPSDIR "/" BOARD_IMAGE_FLIP_NUMBERS, image_numbers);
	if ( !image_letters.Is() )
		Read_From_File( PIXMAPSDIR "/" BOARD_IMAGE_FLIP_LETTERS, image_letters);
	if ( !selection.Is() )
		Read_From_File( PIXMAPSDIR "/" SELECTION_IMAGE, selection);
	if ( !legal.Is() )
		Read_From_File( PIXMAPSDIR "/" LEGAL_MOVE_IMAGE, legal);

}

Server_Image *Get_Image()
{
	return &image;
}

void Draw_Circle(Canvas &canvas, 
		 const Board_Colors &colors,
		 Rect rect, 
		 int circle_color)
{
	canvas.Set_Draw_Mode(Canvas::dm_copy);
	canvas.Set_Color(colors(circle_color, 1));
	canvas.Draw_Ellipse(rect);
	enum {part=23};
	Rect inner(rect.origin + rect.size.x / part,
		   rect.size   - rect.size.x / part * 2);
	canvas.Set_Color(colors(circle_color, 0));
	canvas.Draw_Ellipse(inner);
}

void Draw_Rectangle(Canvas &canvas, 
		 Rect rect)
{
  (void)canvas;
  (void)rect;
}

void Draw_Selection_Rectangle(Canvas &canvas, 
		 Rect rect)
{
	if ( selection.Is() && selection.Get_Alpha_Image().Has_Canvas() ) {
		canvas.Set_Draw_Mode(Canvas::dm_andnot);
    //canvas.Set_Draw_Mode(Canvas::dm_copy);
		canvas.Draw_Canvas(selection.Get_Alpha_Image()(),
			rect,
			Point(0, 0)
			);
		canvas.Set_Draw_Mode(Canvas::dm_or);
		canvas.Draw_Canvas(selection.Get_Image()(),
			rect,
			Point(0, 0)
			);

	}
}

void Draw_Legal_Rectangle(Canvas &canvas, 
		 Rect rect)
{
	if ( legal.Is() && legal.Get_Alpha_Image().Has_Canvas() ) {
		canvas.Set_Draw_Mode(Canvas::dm_andnot);
		canvas.Draw_Canvas(legal.Get_Alpha_Image()(),
			rect,
			Point(0, 0)
			);
		canvas.Set_Draw_Mode(Canvas::dm_or);
		canvas.Draw_Canvas(legal.Get_Image()(),
			rect,
			Point(0, 0)
			);

		canvas.Set_Draw_Mode(Canvas::dm_copy);
	}
}

void Draw_Square_Background(Canvas &canvas,
			    const Board_Colors &colors, 
			    Rect rect,
			    int circle_color,
			    int square_color,
			    int flags,
			    int square,
			    bool selected)
{
  (void)square;
	Canvas::Color bg_color= colors(square_color, 0); 

	canvas.Set_Draw_Mode(Canvas::dm_copy);

	/* The "black" background color is grey by default.  When it
	 * would have been black, draw diagonal stripes, as black
	 * piece on a black background are not visible. 
	 */
	//bool draw_riffles= false;

	/*if (square_color == cc_b && 
	    (flags & bs_riffled || bg_color == Canvas::Get_Black()))
	{
		draw_riffles= true;
		canvas.Set_Color(Canvas::Get_White(),
				 true);
		canvas.Draw_Rect(rect);

	}
	else  * filled background *
	{
		canvas.Set_Color(bg_color, true);
		canvas.Draw_Rect(rect);
	}
	*/

	if ( image.Is() && image.Has_Canvas() ) {
		/*canvas.Draw_Canvas(image(),
			rect, Point(0) );
			*/

		canvas.Draw_Canvas(image(),
			rect,
			Point(rect.origin.x,
			rect.origin.y)
	/*		Point(rect.origin.x + BOARD_IMAGE_LEFT,
			rect.origin.y - BOARD_IMAGE_TOP)
			*/
			);

		/*Get_Size(),
		Point(0));*/
	}
	else  /* filled background */
	{
		canvas.Set_Color(bg_color, true);
		canvas.Draw_Rect(rect);
	}

	/* circle */
	if (circle_color)
	{
		if (bs_tutor) {
			//Draw_Circle(canvas, colors, rect, circle_color); 
			Draw_Legal_Rectangle(canvas, rect); 
		}
	}
	if (selected)
	{
		//Draw_Rectangle(canvas, rect);
		Draw_Selection_Rectangle(canvas, rect);
	}

	/* riffles */
/*	if (draw_riffles)
	{
		enum {line_count= 8};

		canvas.Set_Color(bg_color, true);
		canvas.Set_Line_Mode(((rect.size.x + rect.size.y) / 9 / line_count) >? 1);

		for (int i= 0; i < line_count; ++i)
		{
			canvas.Draw_Line(rect.origin + Point(0, i * rect.size.y / line_count),
					 rect.origin + Point(i * rect.size.x / line_count, 0));
			canvas.Draw_Line(rect.origin + Point(i * rect.size.x / line_count, rect.size.y),
					 rect.origin + Point(rect.size.x, i * rect.size.y / line_count));
		}

		bg_color= Canvas::Get_White();
		* Use white background when drawing the square name,
		 * even on dark squares. 
		 *
	}*/

	/* square name */
	/*
	if (flags & bs_square_names)
	{
		canvas.Set_Color(colors(c_text, 0), true);
		canvas.Set_Color(Canvas::Get_Color_1(), false); 
		canvas.Set_Draw_Mode(Canvas::dm_and);

		canvas.Draw_Label(Point(rect.origin.x,
					rect.origin.y + rect.size.y - Canvas::Get_Descent()),
				  Format("%c%c", 'a'+File(square), '1'+Rank(square)));
	}
	*/
	if (flags & bs_turned)
	{
		Rect rect_letters(20,460,460,20);
		Rect rect_numbers(0,0,20,480);
		//canvas.Set_Draw_Mode(Canvas::dm_copy);
		if ( image_letters.Is() && image_letters.Has_Canvas() )
			canvas.Draw_Canvas(image_letters(),
				rect_letters,
				Point(0,0)
				);

		if ( image_numbers.Is() && image_numbers.Has_Canvas() )
			canvas.Draw_Canvas(image_numbers(),
				rect_numbers,
				Point(0,0)
				);


		/*
		canvas.Set_Draw_Mode(Canvas::dm_copy);
		canvas.Draw_Canvas(image_numbers(),
			rect,
			Point(rect.origin.x,
			rect.origin.y)
			);
		canvas.Set_Draw_Mode(Canvas::dm_copy);
		canvas.Draw_Canvas(image_letters(),
			rect,
			Point(rect.origin.x,
			rect.origin.y)
			);
		*/
	}
}

void Draw_Square(Canvas &canvas,
		 Scaled_Board_Image &scaled,
		 const Board_Colors &colors,
		 Point point,
		 Piece piece,
		 int square_color,
		 int circle_color,
		 int flags,
		 int square,
		 bool selected)
{
	/*Dbg("square %c%c %3d %3d %3d %3d", 
	    'a'+File(square), '1'+Rank(square),
	    square_color, circle_color, flags, piece.Get_Value());
	    */

	Draw_Square_Background(canvas, colors, 
			       Rect(point, scaled.Get_Width()),
			       circle_color,
			       square_color,
			       flags,
			       square,
			       selected);

	if (piece.Is_Empty())  return;

	if (piece.Is() && scaled.Get_Image().Has_Canvas())
	{
		canvas.Set_Draw_Mode(Canvas::dm_copy);

		scaled.Draw(canvas,
			    point, 
			    piece.Get_Type(),
			    piece.Get_Color());
	}
}
