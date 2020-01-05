#ifndef GAME_EDITOR_INCLUDED
#define GAME_EDITOR_INCLUDED

#include "game.hh"
#include "board.hh"
#include "game_stream.hh"
#include "notation_defaults.hh"
#include "preferences.hh"
#include "paner.hh"
#include "packer.hh"
#include "text_button.hh"

#include "promotion_dialog.hh"

#include "chessui_i18n.hh"

class Small_Move
{
public:

	Small_Move()
		{
		}
	Small_Move(const Move &move)
		:  from(move.from),
		   to(move.to),
		   promotion(move.promotion.Is_Empty()
			     ? 0
			     : 4 - move.promotion.Get_Type())
		{
		}
	bool operator == (const Small_Move &small_move)
		{
			return from==small_move.from   &&
				to==small_move.to      &&
				promotion==small_move.promotion;
		}

	int from, to, promotion; 
};

class Game_Editor
//	:  public Document_Editor <Game> 
//	:  public Widget
	:  public V_Packer
{
public:

	const Game &EW_Get_Value();

	bool Open(Pointer <Widget> parent);

	void New() {
		DE_Reset();
	}

	bool Load(gchar* file_name) {
		Game object;
		bool res=true;
		g_print("UI: Loading %s\n",file_name);
		/*if (Check_File_Name(file_name, (Game*)NULL)) {*/
			if (! Read_From_File(file_name, object)) {
				return false;
			}
			else if ( EW_Try_Load(file_name, object) )
      {
      }
			else {
				res=true;
				//res=false;
				//return true;
			}
                        
			if (Widget::Is()) {
				if (! EW_Change_Value(object)) {
					return false;
				}
			} else {
				EW_Set_Value(object);
			}
			// Easiest way to get undo/redo buttons states right.
      
			Go_Backward();
			Go_Forward();
		/*}*/
		return res;
	}

	bool Save(gchar *file_name)
	{
		g_print("UI: Saving %s\n",file_name);

		/*if (Check_File_Name(file_name, (Game*)NULL)) {*/
			if (! Write_To_File(file_name, game))
				return false;
		/*}*/
		return true;
	}
			
	void Go_Forward();
	void Go_Backward();

	void Start_Engine() {
		Engine_Play();
	}

	int Get_Location() {
		return game.Get_Overall_Location();
	}

	int Get_Turn() {
		return game.Get_Position().Get_To_Move();
	}

	void Turn_Board(bool turn) {
		board.Set_Turned(turn);
	}

	void Move_Cursor_Up(void) {
		board.Move_Cursor_Up();
	}
	void Move_Cursor_Down(void) {
		board.Move_Cursor_Down();
	}
	void Move_Cursor_Left(void) {
		board.Move_Cursor_Left();
	}
	void Move_Cursor_Right(void) {
		board.Move_Cursor_Right();
	}
	void Cursor_Action(void) {
		board.Cursor_Action();
	}
	void Cursor_Pos(int pos) {
		board.Cursor_Pos(pos);
	}
	int Get_Cursor_Pos(void) {
		return board.Get_Cursor_Pos();
	}

        void Tutor_Mode(bool tutor) {
                board.Tutor_Mode(tutor);
        }
	void Update_To_Move();        
private:

	Game game;
	bool show_evaluator_widget_on_start;
	bool show_comment;
	//Preferences <Notation, Notation_Editor> notation_preferences;
	Preferences <Notation> notation_preferences;
	//Owned_Editor <Position_Editor> position_editor;
	//Owned_Editor <Game_Info_Editor> game_info_editor;
	int move_list_length;
	/* number of half moves currently shown in game move list.  -1
	 * if nothing is shown.  Used by Update_Move_List_Length(). 
	 */
	int move_list_first;
	Vector <Small_Move> legal_moves;
	/* The index is the same index as in the legal move list.  The
	 * value is the number of that move in the generator. 
	 */

	//Toolbar toolbar;
	//Promotion_Chooser promotion_chooser;
	H_Paner middle_paner;
	//H_Packer middle_paner;
	V_Packer side_packer;
	Text_Button but;
	//Evaluator_Widget evaluator_widget;
	//Material_Widget material_widget;
	Board board;
	//Status_Bar status_bar;
	//H_Packer game_info_packers[2];
	/*Chess_Info_Widget to_move_widgets[2];
	Chess_Info_Widget chess_color_widgets[2];
	*/
	//Label player_names[2];
	//Multi_List_Box game_move_list;
	//Legal_Info_Widget legal_info;
	//Multi_List_Box game_legal_move_list;
	//Framed <V_Packer> comment_frame;
	//Gtk_Scrolled <Multi_Edit> comment;
	//Check_Box comment_diagram; 
	//Option_Menu nag_menu;
	H_Packer event_packer;
	//Label event_text, site_text, round_text; 
	//Label event_text;
	
	Promotion_Dialog *promotion_dialog;
	
	int use_ok;

	void Wdg_Close();

	void MR_Receive(String/*, int*/);

	void Dim(gboolean &undo_dimmed, gboolean &redo_dimmed);
	void Undim(gboolean undo_dimmed, gboolean redo_dimmed);
	bool engine_thinking;

	bool Document_Editor_Open(Pointer <Packer>);
	//bool Document_Editor_Open(Pointer <Widget>);

	/*
	const char *EW_Type_Name() const 
		{
			return _("Chess Game"); 
		}
	const char *EW_Short_Type_Name() const
		{
			return _("The game"); 
		}
		*/

	void EW_Set_Value(Pointer <const Game>);
	int EW_Use()
		{
			return use_ok;
		}
	bool EW_Change_Value(const Game &);
	bool EW_Try_Load(String, Pointer <Game>);
	void EW_Caption_Changed();
	void DE_Reset();
	const char *DE_Get_Program_Name() const
		{
			return "baby_gui_game"; 
		}
	//bool DE_Set_Args(Open_Info &);
	
	void DO_Modified_Changed(bool);

	void Engine_Play();

	void Update_Last_Move();
	void Update_Move_List(); 
	void Update_Move_List_Length();
	/* Set correct length and non-move cells.
	 */
	void Update_Move_List_Highlight();
	void Update_Move_List_Content(int begin= 0, 
				      int end= -1);
	/* Update the move list and the move list selection.   
	 *
	 * From BEGIN to END (-1 means to the end). 
	 */
	//void Update_Legal_List(); 
	void Play(int move_index, bool update= true);
	/* Play the move and update everything.  Start the engine if
	 * settings.reply==re_auto. 
	 * Only do updates if UPDATE. 
	 */
	void Update_Position_Info();
	/* Do everything when the position has changed, except
	 * updating the board, the move list and the variation list.  
	 */
	 /*
	void Go_Forward();
	void Go_Backward();
	*/
	void Save_Comment();
 	void Update_Comment();
	void Update_Game_Info();
	bool Open_Variation_List();
	void Reset_Evaluator();
	void Enter_Variation(int variation_index);
	/* Don't invalidate squares. 
	 */
	void Update_Board_Selection();
	void Update_Location();
	/* Update enabledness of menu entries and tool buttons such as
	 * "forward". 
	 */
	void Update_Variation_Depth();
	/* In the status bar. 
	 */
	void Dragged(); 

	void Set_Thinking() {
		engine_thinking = true;
		board.Set_Thinking();
	}

	void Unset_Thinking() {
		engine_thinking = false;
		board.Unset_Thinking();
	}

};

#endif /* ! GAME_EDITOR_INCLUDED */
