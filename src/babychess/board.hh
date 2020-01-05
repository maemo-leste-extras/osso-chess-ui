#ifndef BOARD_INCLUDED
#define BOARD_INCLUDED

#include "position.hh"
#include "server_image.hh"
#include "square.hh"
//#include "inline.hh"
#include "scaled_board_image.hh"
//#include "context_menu.hh"
//#include "board_settings_editor.hh"
//#include "owned_editor.hh"
//#include "board_settings_stream.hh"
#include "board_defaults.hh"
#include "preferences.hh"
#include "generator.hh"
#include "draw_board.hh"
#include "bit_board.hh"
//#include "menu_bar.hh"

class Board
	:  virtual public Widget
{
public:

	class Open_Info
	{
	public:

		Pointer <const Position> position;
		String msg_move;
		String msg_middle;
		String msg_turn;
		Pointer <const Generator> generator; 
		bool turned;
		//Menu_Bar &menu_bar; 
	
		Open_Info(const Position &new_position,
			  String new_msg_move,
			  String new_msg_middle,
			  String new_msg_turn,
			  Pointer <const Generator> new_generator,
			  bool new_turned/*,
			  Menu_Bar &new_menu_bar*/)
			:  position(new_position),
			   msg_move(new_msg_move),
			   msg_middle(new_msg_middle),
			   msg_turn(new_msg_turn),
			   generator(new_generator),
			   turned(new_turned)/*,
			   menu_bar(new_menu_bar)*/
			{
			}
	};

	void MR_Receive(String);
	bool Open(Pointer <Widget> parent, 
		  const Open_Info &);
	/* MSG_MOVE is sent after a piece was dragged.  The squares
	 * can be retrieved with Get_Squares().  A message is sent
	 * even when the piece was released outside the board.
	 * In this case Get_Squares() returns DRAG_TO=0. 
	 *
	 * When the middle mouse button is clicked, MSG_MIDDLE is sent
	 * and Get_Squares() returns the square in FROM. 
	 */


	bool Get_Squares(int &from, int &to);
	/* After MSG_MOVE, this functions tells the squares.
	 * Return FALSE if no piece was dragged.  
	 */

	const Board_Settings &Get_Settings() const
		{
			return preferences.Get_Value();
		}
	
	void Set_Turned(bool new_turned); 
	bool Get_Turned() const
		{
			return preferences().flags & bs_turned;
		}

	void Set_Last_Move(int square, int square); 
	void Clear_Last_Move()
		{
			Clear_Selection(); 
		}
	void Invalidate_Move_Squares(const Move &);
	void Invalidate_Square(int square,
			       bool interrupt_dragging= true);
	void Invalidate_Position();
	/* All of POSITION has changed. 
	 */
	void Caption_Changed()
		{
			preferences.Caption_Changed(); 
		}
	
	void Set_Thinking() {
		engine_thinking = true;
	}

	void Unset_Thinking() {
		engine_thinking = false;
	}

	void Move_Cursor_Up(void);
	void Move_Cursor_Down(void);
	void Move_Cursor_Left(void);
	void Move_Cursor_Right(void);
	void Cursor_Action(void);
	void Cursor_Pos(int pos);
	int Get_Cursor_Pos(void);

        void Tutor_Mode(bool tutor)
        {
          preferences.Get_Change_Value().flags &= ~bs_tutor;
          if (tutor)
            preferences.Get_Change_Value().flags |= bs_tutor;
        }

private:

	int square_width;
	Scaled_Board_Image scaled_image;
	Server_Image board_image;
	/* The whole board, including the frame if it is visible.  Has
	 * the same size as THIS widget.  
	 */
	//Dynamic <Context_Menu> menu;
	Pointer <const Position> position;
	Pointer <const Generator> generator;
	/* May be NULL.
	 */
	String msg_move;
	String msg_middle;
	String msg_turn;
	bool dragging;
	Point drag_point;
	Point pointer_pos;
	/* Position of pointer inside the dragged square. 
	 */
	Point last_org;
	/* Origin of dragged piece rectangle. 
	 */
	int drag_from;
	int drag_to;
	bool engine_thinking;
	Piece dragged_piece;
	Server_Image drag_img;
	/* Same size as BOARD_IMAGE.  Background image when dragging. 
	 */
	Preferences <Board_Settings/*, Board_Settings_Editor*/> preferences;
	Vector <int> tutor_squares;
	/* Squares highlighted in tutor mode. 
	 */

	int cursorpos;
	int picked;

	int selection[2];
	/* what squares are highlighted as "last move" */
	int last_a, last_b; 
	/* the last move (not shown in yellow if not showing the last
	 * move */ 
	Board_Colors board_colors; 
	Bit_Board invalid;
	//Pointer <Menu_Bar> menu_bar;

	static int frame_width;

	void Change_Sized_Image();
	/* BOARD_IMAGE must be.  Set SQUARE_WIDTH to -1 before calling to
	 * force new loading. 
	 */

	void Draw_Square(Canvas &,
			 int square,
			 int circle_color,
			 bool empty= false); 
	/* Draw that square.  
	 * color=  0:  normal square
 	 *         other:  index of Board_Settings::colors:  draw a
	 *                 circle in that color
	 *
	 * Don't invalidate that region. 
	 */ 

	void Draw_Actual_Square(int square);
	/* draw square as normal square or selected if the square is
	 * selected. 
	 */

	void Draw_Frame();

	int Point_Square(Point) const;
	Rect Square_Rect(int square) const;
	void Interrupt_Dragging();

	void Wdg_Draw(Canvas &);
	void Wdg_Size(Point);
	void Wdg_Mouse(Mouse_Action, Mouse_Button, Point, bool);
	void Wdg_Mouse_Cursor();
	void Wdg_Close();
	void Wdg_Get_Size_Request(int &, int &, bool &); 
	bool Wdg_Key_Press(guint);

	bool Is_Inside_Point(Point p) const;
	void Set_Selection(int square_a, int square_b);
	/* None may be 0. 
	 */
	void Clear_Selection();
	void Update_Selection();
	/*bool Open_Menu(); 
	void Set_Menu();
	void Set_Menu_Turned();
	void Set_Menu_Frame();
	void Set_Menu_Square_Names();
	void Set_Menu_Smooth();
	void Set_Menu_Tutor();
	void Set_Menu_Highlight();
	void Set_Menu_Riffled(); 
	*/
	static const char *Translate(const char *);

	void Move_Cursor(int);
};

/*#define gettext_noop(string) string
#define BOARD_MENU             \
   { gettext_noop("/_Board"),                     NULL,NULL,         0,                          "<Branch>",     0},  \
   { gettext_noop("/Board/_Turn board"),          NULL,MENU_CALLBACK,(int)"board_turn_board",    "<CheckItem>",  0},  \
   { gettext_noop("/Board/_Frame"),               NULL,MENU_CALLBACK,(int)"board_frame",         "<CheckItem>",  0},  \
   { gettext_noop("/Board/_Square names"),        NULL,MENU_CALLBACK,(int)"board_square_names",  "<CheckItem>",  0},  \
   { gettext_noop("/Board/S_mooth chess pieces"), NULL,MENU_CALLBACK,(int)"board_smooth",        "<CheckItem>",  0},  \
   { gettext_noop("/Board/T_utor mode"),          NULL,MENU_CALLBACK,(int)"board_tutor",         "<CheckItem>",  0},  \
   { gettext_noop("/Board/_Highlight last move"), NULL,MENU_CALLBACK,(int)"board_highlight_last","<CheckItem>",  0},  \
   { gettext_noop("/Board/_Riffled dark squares"),NULL,MENU_CALLBACK,(int)"board_riffled",       "<CheckItem>",  0},  \
   { "/Board/sep1",                               NULL,NULL,         0,                          "<Separator>",  0},  \
   { gettext_noop("/Board/_Board preferences"),   NULL,MENU_CALLBACK,(int)"board_preferences",   "<StockItem>",  GTK_STOCK_PREFERENCES}, 
   */

#endif /* ! BOARD_INCLUDED */
