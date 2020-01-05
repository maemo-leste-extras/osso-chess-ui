#include <gdk/gdkkeysyms.h>
#include <libosso.h>

#include "board.hh"
#include "board_image.hh"
#include "client_image.hh"
#include "client_image_stream.hh"
#include "canvas.hh"
#include "dbus.h"
#include "sound.h"
#include "chess_log.h"

#define BOARD_IMAGE_LEFT 24
#define BOARD_IMAGE_TOP 7
#define BOARD_LEFT_FIX 13
int BOARD_TOP=17;
int BOARD_LEFT=0;
int Board::frame_width= -1;

Rect drag_source;

/* Piece is selected by HW Key */
bool is_key_action;

bool Board::Open(Pointer <Widget> parent, 
		 const Open_Info &open_info)
{
	osso_log(LOG_DEBUG,"Board: Open");

	dragging= false;

	msg_move= open_info.msg_move;
	msg_middle= open_info.msg_middle;
	msg_turn= open_info.msg_turn;

	cursorpos = 44;

	osso_log(LOG_DEBUG,"Board: Open: Preferences");
	preferences.Open(*this, "board_settings");
	if (open_info.turned)
		preferences.Get_Change_Value().flags |= bs_turned; 

	position= open_info.position;
	generator= open_info.generator; 
	square_width= -1;
	last_a= last_b= 0; 
	selection[0]= selection[1]= 0;

	osso_log(LOG_DEBUG,"Board: Widget Open");
	if (! Widget::Open(parent))
		return false;

	//menu_bar= open_info.menu_bar;

	osso_log(LOG_DEBUG,"Board: Open: Key signals");
	Connect_Key_Signals(Get());

	if (frame_width < 0)
		frame_width= Canvas::Get_Ascent() + max(unit / 10, 2); 

	BOARD_LEFT = frame_width - BOARD_IMAGE_LEFT;
	BOARD_TOP = frame_width - BOARD_IMAGE_TOP;
	osso_log(LOG_DEBUG,"Board: Open: Read board image");
	::Read_Image();

	osso_log(LOG_DEBUG,"Board: Open: Change size");
	Change_Sized_Image();

	return true;
}

void Board::Wdg_Draw(Canvas &canvas)
{
	osso_log(LOG_DEBUG,"Board: Draw");

	if (! (board_image.Is() && board_image.Has_Canvas()))
		return ;

	for (int rank= 7;  rank >= 0;  --rank)
	{
		int bits= invalid.Get_Rank(rank);
		assert (bits >= 0 && bits < 0x100);
		int file= 0;
		while (bits)
		{
			if (bits & 1)
			{
				Draw_Actual_Square(Make_Square(file, rank));
			}
			bits >>= 1;
			++file;
		}
	}
	invalid.Empty(); 

	board_image().Set_Draw_Mode(Canvas::dm_copy);
	canvas.Draw_Canvas(board_image(),
			   Get_Size(),
			   Point(0));
}

void Board::Change_Sized_Image()
{
	osso_log(LOG_DEBUG,"Board: Change size");

	Interrupt_Dragging();

	int w=36;
	int new_square_width= 1 + max((min(Get_Size().x, Get_Size().y) - w) / 8, 1); 

	if (new_square_width != square_width)
	{
		square_width= new_square_width;
		scaled_image.Load(square_width, preferences().flags & bs_smooth);

		board_image.Free();
		board_image.Create(Get_Size());

		if (board_image.Is())
		{
			Draw_Frame();
			Invalidate_Position(); 
		}
		
		drag_img.Free();
		drag_img.Create(Get_Size()); 
	}

}

void Board::Draw_Square(Canvas &canvas,
			int square,
			int circle_color,
			bool empty)
{
	osso_log(LOG_DEBUG,"Board: Draw square");

	if (! scaled_image.Is())
		return; 

	assert (Is_Inside_Square(square));

	Rect rect= Square_Rect(square);
	Piece piece= position()[square];
	if (empty)  piece= pi_empty; 
	::Draw_Square(canvas, scaled_image, board_colors,
		      rect.origin, piece, Square_Color(square), circle_color,
		      preferences().flags, square, square == cursorpos);
}

void Board::Wdg_Size(Point)
{
	Change_Sized_Image();
}

int Board::Point_Square(Point p) const
{
	osso_log(LOG_DEBUG,"Board: Point square");

	if (preferences().flags & bs_frame)
		p -= frame_width;

	int square= Make_Square(sq_a + ((p.x + BOARD_LEFT) / square_width),
		sq_8 - ((p.y + BOARD_TOP) / square_width));

	if (preferences().flags & bs_turned)
		Square_Turn(square);

	return square;
}

bool Board::Get_Squares(int &from, int &to)
{
	from= drag_from;
	to=   drag_to;

	return true;
}

void Board::Invalidate_Position()
{

	invalid.Set();
	Invalidate();
}

void Board::MR_Receive(String message)
{
	osso_log(LOG_DEBUG,"Board: Receiving message: %s", message());

	if (message == "board_tutor")
	{
		preferences.Get_Change_Value().flags ^= bs_tutor;
		preferences.Changed();
	}
	else if (message == "board_highlight_last")
	{
		preferences.Get_Change_Value().flags ^= bs_highlight_last;
		preferences.Changed();
		Update_Selection(); 
	}
	else if (message == "board_settings")
	{
		Interrupt_Dragging();
		square_width= -1;
		board_colors.Invalidate(); 
		Change_Sized_Image();
		Invalidate(); 
	}
	else if (message == "board_square_names")
	{
		preferences.Get_Change_Value().flags ^= bs_square_names;
		preferences.Changed();
		Invalidate_Position();
	}
	else if (message == "board_turn_board")
	{
		Set_Turned(! Get_Turned());
	}
	else if (message == "board_smooth")
	{
		Interrupt_Dragging();
		square_width= -1;
		preferences.Get_Change_Value().flags ^= bs_smooth; 
		preferences.Changed();
		Change_Sized_Image();
		Invalidate_Position();
	}
	else if (message == "board_frame")
	{
		Interrupt_Dragging();
		preferences.Get_Change_Value().flags ^= bs_frame;
		preferences.Changed(); 
		square_width= -1;
		Change_Sized_Image();
	}

	else 
		Parent_Receive_Message(message);  
}

void Board::Wdg_Mouse_Cursor()
{
	Set_Mouse_Cursor(ms_hand);
}

void Board::Wdg_Mouse(Mouse_Action ma, 
		      Mouse_Button mb, 
		      Point p,
		      bool ac)
{
	if (engine_thinking) return;
		
	if (dragging)
	{
		/* Recalculate cursor position when user selects a piece by HW Key 
		 * and releases that piece by stylus. 
		*/
		if (is_key_action) {
		   Rect rect= Square_Rect(Point_Square(p));
	      p = rect.origin;
		}

		if (ma == ma_move)
			/* move pointer while dragging */
		{
			Point new_org= p - pointer_pos;

			/* temporary image */ 
			static Server_Image i;
			static int i_size= -1;

			if (i_size != square_width)
			{
				if (i.Is())
					i.Free();
				i.Create(Point(square_width));
				i_size= square_width;
			}

			if (i.Is() 
			    && i.Has_Canvas()
			    && drag_img.Has_Canvas()
			    && board_image.Has_Canvas()
			    && scaled_image.Get_Image().Has_Canvas())
			{
				/* draw background on board on old
				   location */ 
				board_image().Set_Draw_Mode(Canvas::dm_copy);
				board_image().Draw_Canvas
					(drag_img(),
					 Rect(last_org, Point(square_width)),
					 last_org);

				/* draw image into temporary */
				i().Set_Draw_Mode(Canvas::dm_copy); 
				i().Draw_Canvas(board_image(),
						Rect(Point(square_width)),
						new_org);
				/* draw new piece on temporary */ 
				scaled_image.Draw(i(),
						  Point(0),
						  dragged_piece.Get_Type(),
						  dragged_piece.Get_Color());
				/* draw temporary on board */ 
				board_image().Draw_Canvas
					(i(),
					 Rect(new_org, Point(square_width)),
					 Point(0));
				Invalidate(Rect(last_org, Point(square_width)));
				Invalidate(Rect(new_org,  Point(square_width)));

				last_org= new_org;

				//::Draw_Rectangle(board_image(), drag_source);
				::Draw_Selection_Rectangle(board_image(), drag_source);
			}
		}
		else if (mb == mb_left && (ma == ma_up || ma == ma_down) && ac)
			/* end dragging */
		{
			osso_log(LOG_DEBUG,
				"Board: Mouse handling: End dragging");
				
			/* P is used to calculate the release square,
			 * but the piece is still drawn at LAST_ORG. 
			 */
			Interrupt_Dragging(); 

			/* Reset key action flag state */
  			is_key_action = false;
			
			Point middle= p - pointer_pos + Point((square_width+2) / 2);
			/* Center of the dragged piece. 
			 */
			int frame_w= 0;
			if (preferences().flags & bs_frame)
			{
				middle -= frame_width;
				middle.x += BOARD_LEFT;
				/*middle.y += BOARD_TOP;
				*/
				frame_w= frame_width; 
			}
			if (middle.x < 0 || middle.x >= 8 * square_width  ||
			    middle.y < 0 || middle.y >= 8 * square_width)
			{
			Invalidate_Square(cursorpos, false);
				sound_play(get_app_data()->app_sound_data, SOUND_MOVE_FAIL);
				return; 
			}
			int file= sq_a + middle.x / square_width;
			int rank= sq_8 - middle.y / square_width;

			/* Square of release. 
			 */
			drag_to= Make_Invalid_Square(file, rank);
			if (preferences().flags & bs_turned)
				Square_Turn(drag_to);
			
			if (! Is_Inside_Square(drag_to))
				drag_to= 0;

			Parent_Send_Message(msg_move);

			Invalidate_Square(cursorpos, false);
			cursorpos = drag_to;
			Invalidate_Square(cursorpos, true);

			//::Draw_Selection_Rectangle(board_image(), Rect(drag_to, Point(square_width)));
			::Draw_Rectangle(board_image(), Rect(drag_to, Point(square_width)));
			Invalidate_Square(cursorpos, true);
		}
	}
	else
	{
		if (mb == mb_left && ma == ma_down && ac)
			/* begin dragging */
		{
			osso_log(LOG_DEBUG,"Board: Mouse handling: Dragging");
			if (! (drag_img.Is() && board_image.Is()))   return;
			if (! (drag_img.Has_Canvas() && board_image.Has_Canvas()))   return;
			if (! Is_Inside_Point(p))  return;

			drag_from= Point_Square(p);

			if (! Is_Inside_Square(drag_from))
				return;
			dragged_piece= position/*->Get_Piece*/()[drag_from];
			if (! dragged_piece.Is())
				/* Drag an empty square. 
				 */ 
			{
				sound_play(get_app_data()->app_sound_data, SOUND_SELECT_FAIL);
				drag_to= drag_from;
//				Parent_Send_Message(msg_move);

				return;
			}

			/* Color selection?*/
			AppData *app_data = get_app_data();
			if (! dragged_piece.Is_Piece_Of_Color(
			      app_data->app_ui_data->game->Get_Turn()
			      )
			    )
			{
				sound_play(get_app_data()->app_sound_data, SOUND_SELECT_FAIL);
			   	return;
			}
			/* */

			gtk_widget_grab_focus(Get());
			sound_play(get_app_data()->app_sound_data, SOUND_SELECT);

			/* Remove old focus*/
			int oldpos = cursorpos;
			cursorpos = drag_from;
			// Invalidate_Square(drag_from, false);
			Draw_Actual_Square(oldpos);
			Invalidate(Square_Rect(oldpos));

			/* draw from square */ 
			Draw_Square(board_image(), 
				    drag_from,
				    c_highlight);
			Rect rect= Square_Rect(drag_from);
			Invalidate(rect); 
		   
			/* draw tutor squares */ 
			tutor_squares= 0;
			if (generator.Is() && (preferences().flags & bs_tutor) && generator().Is())
			{
				int index;
				for (index= 0;  index < *generator();  ++index)
				{
					if (generator()[index].from == drag_from)
						break;
				}
				if (index < *generator())
				{
					int end= index;
					do 
					{
						++end;
					} while (end < *generator() && generator()[end].from == drag_from);
					if (! tutor_squares.Set(end-index))
						return;
					for (int i= 0;  i < end-index;  ++i)
					{
						int to= generator()[i+index].to;
						assert (to != drag_from);
						assert (Is_Inside_Square(to));
						tutor_squares[i]= to;
						if (generator()[i+index].promotion.Is())
						{
							tutor_squares= -3;
							index += 3;
						}
						/* draw square */
						Draw_Square(board_image(), to, c_highlight);
						Rect rect= Square_Rect(to);
						Invalidate(rect);
					}
				}
			}

			drag_img->Set_Draw_Mode(Canvas::dm_copy);
			drag_img->Draw_Canvas(board_image(),
					      Rect(Get_Size()), 
					      Point(0));
			Draw_Square(drag_img(), drag_from, c_highlight, true);

			int w= preferences().flags & bs_frame ? frame_width : 0;

			last_org= Point((p.x - w + BOARD_LEFT) / (square_width) * (square_width) + w - BOARD_LEFT,
					(p.y - w + BOARD_TOP) / (square_width) * (square_width) + w - BOARD_TOP );

			pointer_pos= p - last_org;

			dragging= true;

			drag_source = rect;
			//::Draw_Rectangle(board_image(), rect);
		}
	}
}

void Board::Interrupt_Dragging()
{
	if (! dragging)
		return;

	dragging= false;

	/* redraw current position of dragged piece */ 
	Rect rect(last_org, Point(square_width));
	if (rect.origin.x < 0)
	{
		rect.size.x += rect.origin.x;
		rect.origin.x= 0;
	}
	if (rect.origin.y < 0)
	{
		rect.size.y += rect.origin.y;
		rect.origin.y= 0;
	}

	Point osize= board_image.Get_Size();
	if (rect.origin.x + rect.size.x > osize.x)
		rect.size.x= osize.x - rect.origin.x;
	if (rect.origin.y + rect.size.y > osize.y)
		rect.size.y= osize.y - rect.origin.y;
	board_image().Set_Draw_Mode(Canvas::dm_copy); // prevent ghost image
	board_image().Draw_Canvas(drag_img(), rect, rect.origin);
	
	/* invalidate square around DRAG_TO
	 * that were obscured by piece */ 
	Invalidate(rect);

	/* redraw from square */
	Invalidate_Square(drag_from); 

	/* unselect tutor squares */
	if (*tutor_squares)
	{
		for (int i= 0;  i < *tutor_squares;  ++i)
		{
			int square= tutor_squares[i];

			invalid.Set(square);
			Rect rect= Square_Rect(square);
			Invalidate(rect);
		}
		tutor_squares= 0;
	}
}

Rect Board::Square_Rect(int square) const
{
	int file= File(square);
	int rank= Rank(square); 
	if (preferences().flags & bs_turned)
	{
		file= sq_h - (file - sq_a);
		rank= sq_8 - (rank - sq_1);
	}

	int w= preferences().flags & bs_frame ? frame_width : 0;

	return Rect(
		(square_width) * (file - sq_a) + w - BOARD_LEFT,
		(square_width) * (sq_8 - rank) + w - BOARD_TOP,
		    square_width, square_width);
}

void Board::Wdg_Close()
{
	osso_log(LOG_DEBUG,"Board: Close");

	preferences.Close(); 
	position= NULL;
	generator= NULL;
	Widget::Wdg_Close();
}

void Board::Set_Selection(int a, int b)
{
	Interrupt_Dragging();

	Clear_Selection(); 

	{
		selection[0]= a;
		invalid.Set(a);
		Rect rect= Square_Rect(a);
		Invalidate(rect);
	}

	{
		selection[1]= b;
		invalid.Set(b);
		Rect rect= Square_Rect(b);
		Invalidate(rect);
	}
}

void Board::Clear_Selection()
{
	if (selection[0] != 0)
	{
		int square= selection[0];
		selection[0]= 0;
		invalid.Set(square);
		Rect rect= Square_Rect(square);
		Invalidate(rect);
	}
	if (selection[1] != 0)
	{
		int square= selection[1];
		selection[1]= 0;	
		invalid.Set(square);
		Rect rect= Square_Rect(square); 
		Invalidate(rect);
	}
}

void Board::Wdg_Get_Size_Request(int &, 
			    int &,
			    bool &resizeable)
{
	//resizeable= true; 
	resizeable= false; 
}

void Board::Invalidate_Square(int square,
			      bool interrupt_dragging)
{
	assert (Is_Inside_Square(square));

	if (interrupt_dragging)
		Interrupt_Dragging(); 

	invalid.Set(square);
  
	Rect rect= Square_Rect(square);
	Invalidate(rect);
}

void Board::Set_Turned(bool new_turned) 
{
	Interrupt_Dragging();
	if (new_turned)
		preferences.Get_Change_Value().flags |= bs_turned;
	else
		preferences.Get_Change_Value().flags &= ~bs_turned; 
	preferences.Changed();
	square_width= -1;
	Change_Sized_Image();

	Parent_Send_Message(msg_turn);
}

void Board::Draw_Frame()
{
	osso_log(LOG_DEBUG,"Board: Drawing frame");

	if (! (preferences().flags & bs_frame))
		return;

	if (! board_image.Is())
		return;
	if (! board_image.Has_Canvas())
		return;

	Canvas &canvas= board_image.Get_Canvas();

	board_colors.Make(preferences()); 

	canvas.Set_Draw_Mode(Canvas::dm_copy);
	canvas.Set_Color(preferences().colors[c_frame]);

	Server_Image *image = ::Get_Image();
	if ( image->Is() && image->Has_Canvas() ) {

		canvas.Draw_Canvas((*image)(),
			image->Get_Size(),
			Point(0)
			);
	} else {
	canvas.Draw_Rect(Rect(0, 
			      0,
			      2 * frame_width + 8 * square_width,
			      frame_width));
	canvas.Draw_Rect(Rect(0,
			      frame_width + 8 * square_width,
			      2 * frame_width + 8 * square_width,
			      frame_width));
	canvas.Draw_Rect(Rect(0,
			      frame_width,
			      frame_width,
			      8 * square_width));
	canvas.Draw_Rect(Rect(frame_width + 8 * square_width,
			      frame_width,
			      frame_width,
			      8 * square_width));

	canvas.Set_Color(board_colors(c_text, 0)); 

	int ascent= Canvas::Get_Ascent();

	for (int file= 0;  file < 8;  ++file)
	{
		String text(preferences().flags & bs_turned 
			    ? 'H' - file
			    : 'A' + file);
		int width= Canvas::Get_Label_Width(text);
		int x= frame_width + ((2 * file + 1) * square_width - width) / 2;
		int y= (frame_width - ascent) / 2 + ascent;
		canvas.Draw_Label(Point(x, y), text);
		y += frame_width + 8 * square_width; 
		canvas.Draw_Label(Point(x, y), text);
	}

	for (int rank= 0;  rank < 8;  ++rank)
	{
		String text(preferences().flags & bs_turned
			    ? '1' + rank
			    : '8' - rank);
		int width= Canvas::Get_Label_Width(text);
		int y= frame_width + rank * square_width + (square_width - ascent) / 2 + ascent; 
		int x= (frame_width - width) / 2;
		canvas.Draw_Label(Point(x, y), text);
		x += frame_width + 8 * square_width;
		canvas.Draw_Label(Point(x, y), text);
	}
	}
}

void Board::Invalidate_Move_Squares(const Move &move)
{
  //Dbg("Board::Invalidate_Move_Squares(%d-%d)", move.from, move.to);

	int changed[4];
	move.Squares_Touched(changed);
	Invalidate_Square(changed[0]);
	Invalidate_Square(changed[1]);
	if (changed[2])
		Invalidate_Square(changed[2]);
	if (changed[3])
		Invalidate_Square(changed[3]);
}

bool Board::Is_Inside_Point(Point p) const
{
	int w= preferences().flags & bs_frame ? frame_width : 0;
	return (p.x >= w && p.x < 8 * square_width + w + BOARD_LEFT_FIX &&
		p.y >= w - BOARD_TOP && p.y < 8 * square_width + w);
}

bool Board::Wdg_Key_Press(guint keyval)
{
	if (keyval == GDK_Escape)
	{
		Interrupt_Dragging(); 
		return true;
	}
	else
	{
		return false;
	}
}

void Board::Set_Last_Move(int square_a, int square_b)
{
	last_a= square_a;
	last_b= square_b;
	if (preferences().flags & bs_highlight_last)
		Set_Selection(square_a, square_b);
	else
		Clear_Selection(); 
}

void Board::Update_Selection()
{
	if (preferences().flags & bs_highlight_last)
	{
		if (last_a && last_b)
			Set_Selection(last_a, last_b);
	}
	else
	{
		Clear_Selection(); 
	}
}

void Board::Draw_Actual_Square(int square)
{
	if (! board_image.Has_Canvas())
		return;
	Draw_Square(board_image(),
		    square,
		    (selection[0] == square || selection[1] == square) 
		    ? c_last_move : 0); 
}

void Board::Move_Cursor_Up(void)
{
	int oldpos = cursorpos;
	if (preferences.Get_Change_Value().flags & bs_turned)
		cursorpos -= 10;
	else
		cursorpos += 10;
	Move_Cursor(oldpos);
}

void Board::Move_Cursor_Down(void)
{
	int oldpos = cursorpos;
	if (preferences.Get_Change_Value().flags & bs_turned)
		cursorpos += 10;
	else
		cursorpos -= 10;
	Move_Cursor(oldpos);
}

void Board::Move_Cursor_Left(void)
{
	int oldpos = cursorpos;
	if (preferences.Get_Change_Value().flags & bs_turned)
		cursorpos += 1;
	else
		cursorpos -= 1;
	Move_Cursor(oldpos);
}

void Board::Move_Cursor_Right(void)
{
	int oldpos = cursorpos;
	if (preferences.Get_Change_Value().flags & bs_turned)
		cursorpos -= 1;
	else
		cursorpos += 1;
	Move_Cursor(oldpos);
}

void Board::Move_Cursor(int oldpos)
{
	if (cursorpos % 10 == 0)
		cursorpos += 8;
	if (cursorpos % 10 == 9)
		cursorpos -= 8;
	if (cursorpos / 10 == 1)
		cursorpos += 80;
	if (cursorpos / 10 == 10)
		cursorpos -= 80;
	if (!dragging)
	{
		Invalidate_Square(oldpos, false);
		Invalidate_Square(cursorpos, false);
	} else {
		Rect rect= Square_Rect(cursorpos);
		Wdg_Mouse(ma_move, mb_left, rect.origin, true);
		::Draw_Rectangle(board_image(), rect);
	}
}

void Board::Cursor_Action(void)
{
	Rect rect= Square_Rect(cursorpos);
	/*int temp = cursorpos;
	cursorpos = -1;
	*/
	
	/* Select a piece by HW Key */
	if (!dragging) 
	  is_key_action = true;
	  
	Wdg_Mouse(ma_down, mb_left, rect.origin, true);
	//::Draw_Rectangle(board_image(), rect);
	//cursorpos = temp;
}

int Board::Get_Cursor_Pos(void)
{
	return cursorpos;
}
void Board::Cursor_Pos(int pos)
{
	cursorpos = pos;
}
