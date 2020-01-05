
#include "game_editor.hh"
#include "fen.hh"
#include "truncate.hh"
#include "nag.hh"
#include "pgn.hh"
#include "notation_reset.hh"
#include "stream_file.hh"
#include "position_stream.hh"

#include "dbus.h"
#include "chess_enginecommunication.hh"
#include "sound.h"

#include <libintl.h>
#include <libosso.h>

extern gboolean stop_reading;
extern gboolean continue_called;

bool
Game_Editor::Open(Pointer < Widget > parent)
{
    osso_log(LOG_DEBUG, "Game editor: Open");

    game.New();
    if (game.Is())
    {
        EW_Set_Value(game);
    }

    if (!V_Packer::Open(parent))
    {
        return false;
    }

    if (!Document_Editor_Open(*this))
    {
        V_Packer::Close(true);
        return false;
    }

    promotion_dialog = NULL;

    return true;
}

bool
Game_Editor::Document_Editor_Open(Pointer < Packer > parent)
// bool Game_Editor::Document_Editor_Open(Pointer <Widget> parent)
{
    osso_log(LOG_DEBUG, "Game editor: Document editor open");

    notation_preferences.Open(*this, "notation");
    move_list_length = -1;
    move_list_first = -1;

#ifndef gettext_noop
#define gettext_noop(string)  string
#endif

    osso_log(LOG_DEBUG, "Game editor: Setting up board");

    parent().Set_Next_Resizeable(true);
    /* middle packer */
    if (!middle_paner.Open(parent()))
        return false;

    /* board */
    osso_log(LOG_DEBUG, "Game editor: Opening board");
    if (!board.Open(middle_paner, Board::Open_Info(game.Get_Position(), "dragged", "",  /* -1 
                                                                                         * , */
                                                   "msg_board_turn",
                                                   game.Get_Generator(),
                                                   false)))
        return false;

    osso_log(LOG_DEBUG, "Game editor: Update");
    /* 
     * Update_Position_Info(); Update_Move_List(); Update_Location();
     * Update_Game_Info(); */

    return true;
}

void
Game_Editor::MR_Receive(String message)
{
    osso_log(LOG_DEBUG, "Game editor: Receiving message: %s", message());

    if (!memcmp(message(), "board_", 6))
        board.MR_Receive(message);

    else if (message == "remove_end")
    {
        game.Remove_End();
        Update_Move_List_Length();
        // Modify();
    }
    else if (message == "back")
    {
        Go_Backward();
    }
    else if (message == "end")
    {
        Save_Comment();
        if (!game.Go_To_End())
            return;
        Reset_Evaluator();
        Update_Position_Info();
        Update_Move_List_Highlight();
        Update_Location();
        board.Invalidate_Position();
    }
    else if (message == "notation")
    {
        Update_Move_List();
        // Update_Legal_List(); 
        // evaluator_widget.Set_Notation(notation_preferences());
        // promotion_chooser.Update(notation_preferences()); 
    }
    else if (message == "dragged")
    {
        Dragged();
    }
    else if (message == "forward")
    {
        Go_Forward();
    }
    else if (message == "start")
    {
        Save_Comment();
        int depth = game.Get_Variation_Depth();
        int length = game.Get_Location_Stack()[0];
        assert(depth >= 1);
        if (game.Go_To_Start())
        {
            Reset_Evaluator();
            Update_Position_Info();
            if (depth == 1)
                ;
            else
            {
                Update_Move_List_Length();
                Update_Move_List_Content(length);
            }
            Update_Move_List_Highlight();
            Update_Location();
            board.Invalidate_Position();
        }
    }
    else if (message == "game_info_edited")
    {
        Update_Game_Info();
        // Modify();
    }
    else if (message == "ew_play")
    {
        if (*game.Get_Generator() == 0)
            return;

        return;
    }
    else if (message == "variation_remove")
    {
        game.Remove_Variation();
        Update_Position_Info();
        Update_Move_List_Length();
        Update_Move_List_Content(game.Get_Overall_Location());
        Update_Move_List_Highlight();
        Update_Location();
        board.Invalidate_Position();
    }
    else if (message == "variation_leave")
    {
        Reset_Evaluator();
        if (!game.Leave_Variation())
            return;
        Update_Move_List_Length();
        Update_Move_List_Content(game.Get_Overall_Location());
        Update_Move_List_Highlight();
        Update_Location();
        Update_Position_Info();
        board.Invalidate_Position();
    }
    else if (message == "variation_make_parent")
    {
        if (game.Make_Parent())
        {
            Update_Move_List_Content();
            Update_Variation_Depth();
            // Modify();
        }
    }
    else if (message == "variation_make_main")
    {
        if (game.Make_Main())
        {
            Update_Move_List_Content();
            Update_Variation_Depth();
            // Modify(); 
        }
    }
}

void
Game_Editor::Wdg_Close()
{
    osso_log(LOG_DEBUG, "Game editor: Closing");

    Reset_Evaluator();
    // position_editor.Close();
    notation_preferences.Close();
    // game_info_editor.Close(); 
    Save_Comment();
    // Document_Editor <Game> ::Wdg_Close();
}

void
Game_Editor::EW_Set_Value(Pointer < const Game > value)
{
    /* If the game was opened through options, ignore VALUE. */
    if (game.Is())
        return;

    if (!value)
        game.New();
    else
        game = value();
}

void
Game_Editor::Update_To_Move()
{
    AppData *app_data = get_app_data();

    /* Change player status */
    if (app_data != NULL && app_data->app_ui_data != NULL &&
        app_data->app_ui_data->white_status != NULL &&
        app_data->app_ui_data->black_status != NULL)
        if (game.Get_Position().Get_To_Move() == cc_w)
        {
            if (board.Get_Turned())
            {
                app_data->app_ui_data->white_status->Set_Active(false);
                app_data->app_ui_data->black_status->Set_Active(true);
            }
            else
            {
                app_data->app_ui_data->white_status->Set_Active(true);
                app_data->app_ui_data->black_status->Set_Active(false);
            }
        }
        else
        {
            if (board.Get_Turned())
            {
                app_data->app_ui_data->white_status->Set_Active(true);
                app_data->app_ui_data->black_status->Set_Active(false);
            }
            else
            {
                app_data->app_ui_data->white_status->Set_Active(false);
                app_data->app_ui_data->black_status->Set_Active(true);
            }
        }
}

void
Game_Editor::Update_Last_Move()
{
    /* if (status_bar.Is(0)) { String text=
     * game.Format_Last_Move(notation_preferences()); Dbg_Str(text());
     * status_bar.Set(0, text); } */

    /* int location= game.Get_Overall_Location(); if (location != 0) {
     * nag_menu.Set_Enabled(true); int nag= game.Get_Overall_NAG(location -
     * 1); assert (nag >= 0); int s= nag <? nag_count+1;
     * nag_menu.Set_Selected(s); } else { nag_menu.Set_Selected(0);
     * nag_menu.Set_Enabled(false); } */
}

const Game &
Game_Editor::EW_Get_Value()
{
    if (Is())
        Save_Comment();
    return game;
}

void
Game_Editor::Play(int move_index, bool update)
{
    osso_log(LOG_DEBUG, "Game editor: Play");

    Engine *engine = get_chess_engine();
    engine->set_force();
    Move move = game.Get_Generator()[move_index];

    /* move is next in current variation: go forward */

    if (game.Get_Current_Length() > game.Get_Current_Location() &&
        (game.Get_Current_Move(game.Get_Current_Location())
         == game.Get_Generator()[move_index]))
    {
        engine->unset_force();
        Go_Forward();
        return;
    }

    AppData *app_data = get_app_data();
    if (app_data != NULL && app_data->app_ui_data != NULL)
    {
        app_data->app_ui_data->undo_button->Dimmed(FALSE);
        app_data->app_ui_data->redo_button->Dimmed(TRUE);
    }

    /* Move is a variation */

    if (game.Get_Current_Variation().
        Have_Variations(game.Get_Current_Location()))
    {
        for (int i = 0;
             i < *game.Get_Current_Variation().Get_Variations
             (game.Get_Current_Location()); ++i)
        {
            const Variation & variation =
                game.Get_Current_Variation().Get_Variations(game.
                                                            Get_Current_Location
                                                            ())[i] ();

            if (variation.Get_Length() != 0 && move == variation[0])
            {
                Enter_Variation(i);
                board.Invalidate_Move_Squares(move);
                return;
            }
        }
    }

    if (game.Is_End())
    {
        game.Remove_End();
    }

    /* Play the move */

    Reset_Evaluator();

    Move_Name move_name = game.Get_Generator().Get_Move_Name(move_index);


    Save_Comment();
    if (game.Get_Current_Location() !=
        game.Get_Current_Variation().Get_Length())
        game.Append_Variation();

    game.Append_Move(move_index);

    /* update */

    if (!update)
        return;
    board.Invalidate_Move_Squares(move);
    Update_Move_List_Length();
    Update_Move_List_Content(game.Get_Overall_Location() - 1);
    Update_Move_List_Highlight();
    Update_Location();
    Update_Position_Info();

    // Modify();
    /* If the move ends the main variation with mate or stalemate, change
     * game info. */
    if (move_name.force && game.Get_Variation_Depth() == 1)
    {
        Game_Info::Result result;

        if (move_name.force)
        {
            if (move_name.check)
                result = (game.Get_Position().Get_To_Move() == cc_w
                          ? Game_Info::re_black : Game_Info::re_white);
            else
                result = Game_Info::re_draw;
        }

        game.Get_Game_Info().result = result;
    }

    gboolean dialogshown = FALSE;

    if (game.Get_Overall_Length() > 0)
    {
        Engine *engine = get_chess_engine();
        gchar *move =
            g_strdup(game.Get_Overall_Move_Text(game.Get_Overall_Length() - 1,
                                                notation_preferences())());
        int computer = cc_b;
        if (!app_data->app_ui_data->player_color_white)
            computer = cc_w;
        if (!engine->is_human() && computer == Get_Turn())
        {
            Set_Thinking();
            gboolean undo_dimmed = FALSE, redo_dimmed = FALSE;
            Dim(undo_dimmed, redo_dimmed);

            engine->move(move);

            if (gtk_events_pending())
                gtk_main_iteration();


            Undim(undo_dimmed, redo_dimmed);
            Unset_Thinking();
        }
        else if (engine->is_human())
        {
            Move_Name name =
                game.Get_Overall_Move_Name(game.Get_Overall_Length() - 1);
            if (name.check > 0)
            {
                if (name.force > 0)
                {
                    if (Get_Turn() == cc_w)
                        chess_end(cr_black);
                    else
                        chess_end(cr_white);
                }
            }
            else if (name.force > 0)
            {
                dialogshown = TRUE;
                chess_end(cr_draw);
            }
        }

        if (move_name.force)
        {
            if (!dialogshown)
            {
                sound_play(get_app_data()->app_sound_data, SOUND_MATE);

                chess_check_mate();
            }
        }
        else if (move_name.check)
        {
            sound_play(get_app_data()->app_sound_data, SOUND_CHECK);

            chess_check();
        }
        else if (move_name.capture)
        {
            sound_play(get_app_data()->app_sound_data, SOUND_CAPTURE);
        }
        else
        {
            sound_play(get_app_data()->app_sound_data, SOUND_MOVE);
        }

        g_free(move);
    }

}

bool
Game_Editor::EW_Change_Value(const Game & new_game)
{
    game = new_game;
    Reset_Evaluator();
    Update_Move_List();
    Update_Location();
    Update_Position_Info();
    board.Invalidate_Position();
    Update_Game_Info();
    return true;
}

bool
Game_Editor::EW_Try_Load(String file_name, Pointer < Game > game)
{
    Position position;
    if (Read_From_File(file_name, position))
    {
        game().New(position);
        return true;
    }
    else
        return false;
}

void
Game_Editor::Update_Position_Info()
{
    Update_Board_Selection();
    Update_Last_Move();
    Update_To_Move();
    // Update_Legal_List(); 
    // Update_Comment();

    /* variation depth */
    Update_Variation_Depth();
}

void
Game_Editor::DE_Reset()
{

    Reset_Evaluator();

    game.New();

    AppData *app_data = get_app_data();
    if (app_data != NULL && app_data->app_ui_data != NULL)
    {
        app_data->app_ui_data->undo_button->Dimmed(TRUE);
        app_data->app_ui_data->redo_button->Dimmed(TRUE);
    }

    promotion_dialog = NULL;

    if (gtk_events_pending())
        gtk_main_iteration();


    Unset_Thinking();

    Update_Position_Info();
    Update_Move_List();
    Update_Location();
    board.Invalidate_Position();
    Update_Game_Info();
}

void
Game_Editor::Update_Move_List_Length()
{
    AppData *app_data = get_app_data();
    if (app_data == NULL)
        return;
    if (app_data->app_ui_data == NULL)
        return;

    int new_length = game.Get_Overall_Length();
    if (new_length == 0)
        app_data->app_ui_data->moves_list->Clear();
    else
        app_data->app_ui_data->moves_list->Resize(new_length);
}

void
Game_Editor::Update_Move_List_Content(int begin, int end)
{
    /* moves */
    AppData *app_data = get_app_data();
    if (app_data == NULL)
        return;
    if (app_data->app_ui_data == NULL)
        return;

    if (end < 0)
        end = game.Get_Overall_Length();
    for (int i = begin; i < end; ++i)
    {
        gchar *tmp =
            g_ascii_strup(Simple_Notation(game.Get_Overall_Move(i)) (), -1);
        app_data->app_ui_data->moves_list->Update(i, tmp);
        g_free(tmp);
        /* app_data->app_ui_data->moves_list->Update( i,
         * game.Get_Overall_Move_Text(i, notation_preferences())() ); */
    }
}

void
Game_Editor::Update_Move_List_Highlight()
{
}

void
Game_Editor::Go_Backward()
{
    stop_reading = FALSE;

    Save_Comment();

    if (game.Get_Overall_Location() == 0)
        return;

    Move move = game.Get_Overall_Move(game.Get_Overall_Location() - 1);
    Move move2;
    Engine *engine = get_chess_engine();

    AppData *app_data = get_app_data();

    if ((game.Get_Overall_Location() <= 2) && !engine->is_human() 
        && !app_data->app_ui_data->player_color_white)
        return;

    if (!engine->is_human())
        move2 = game.Get_Overall_Move(game.Get_Overall_Location() - 2);

    int depth = game.Get_Variation_Depth();
    if (!game.Go_Backward())
        return;

    app_data->app_ui_data->mate_pos = FALSE;
    if (app_data != NULL && app_data->app_ui_data != NULL)
    {
        gint limit = 1;
        app_data->app_ui_data->redo_button->Dimmed(FALSE);
        if (!app_data->app_ui_data->player_color_white)
            limit = 2;
        if (engine->is_human())
            limit = 0;
        if (game.Get_Overall_Location() <= limit)
        {
            app_data->app_ui_data->undo_button->Dimmed(TRUE);
        }
    }

    // human
    if (!engine->is_human())
    {
        game.Go_Backward();
        engine->remove();       // Undo two moves
    }
    else
    {
        // engine->undo(); //Undo last move
    }

    Reset_Evaluator();
    Update_Position_Info();
    board.Invalidate_Move_Squares(move);    /* squares of move taken back */
    if (!engine->is_human())
    {
        board.Invalidate_Move_Squares(move2);
    }
    if (depth != game.Get_Variation_Depth())
    {
        Update_Move_List_Length();
        Update_Move_List_Content(game.Get_Overall_Location());
        Update_Move_List_Highlight();
    }
    else
        Update_Move_List_Highlight();
    Update_Location();
}


void
Game_Editor::Save_Comment()
{
}

void
Game_Editor::Update_Comment()
{
}

void
Game_Editor::Go_Forward()
{
    Save_Comment();
    AppData *app_data = get_app_data();
    if (!game.Go_Forward())
    {
        app_data->app_ui_data->redo_button->Dimmed(TRUE);
        return;
    }
    Engine *engine = get_chess_engine();
    if (!engine->is_human())
    {
        if (!game.Go_Forward())
            app_data->app_ui_data->redo_button->Dimmed(TRUE);
    }
    if (!game.Is_End())
    {
        if (app_data != NULL && app_data->app_ui_data != NULL)
        {
            app_data->app_ui_data->redo_button->Dimmed(TRUE);
        }

    }
    app_data->app_ui_data->undo_button->Dimmed(FALSE);
    Reset_Evaluator();
    Update_Position_Info();
    Update_Move_List_Highlight();
    Update_Location();

    board.Invalidate_Move_Squares(game.
                                  Get_Overall_Move(game.
                                                   Get_Overall_Location() -
                                                   1));
    /* Update engine status, manual redo... */
    if (!engine->is_human())
    {
        String tmp;

        if (game.Get_Overall_Location() > 1)
        {
            /* Update board as well */
            board.Invalidate_Move_Squares(game.
                                          Get_Overall_Move(game.
                                                           Get_Overall_Location
                                                           () - 2));
            /* Make the moves for white and black */
            tmp = game.Get_Overall_Move_Text(game.Get_Overall_Location() - 2,
                                             notation_preferences());
            engine->move(tmp());
        }

        /* fast repeating redo/undo can lead to a state with
         * Get_Overall_Location returning 0 */
        if (game.Get_Overall_Location() > 0)
        {
                tmp = game.Get_Overall_Move_Text(game.Get_Overall_Location() - 1,
                                                 notation_preferences());
                engine->move(tmp());
        }
    }
    return;
}

void
Game_Editor::Update_Game_Info()
{
    gboolean player_is_black =
        !get_app_data()->app_ui_data->player_color_white;
    gboolean opponent_human =
        get_app_data()->app_ui_data->opponent_type_human;

    if (player_is_black)
    {
        game.Get_Game_Info().black = CHESS_PLAYER1_NAME;
        game.Get_Game_Info().white =
            (opponent_human ? CHESS_PLAYER2_NAME : CHESS_COMPUTER_NAME);
    }
    else
    {
        game.Get_Game_Info().white = CHESS_PLAYER1_NAME;
        game.Get_Game_Info().black =
            (opponent_human ? CHESS_PLAYER2_NAME : CHESS_COMPUTER_NAME);
    }
}

void
Game_Editor::Reset_Evaluator()
{
}

void
Game_Editor::Enter_Variation(int index)
{
    Reset_Evaluator();
    Save_Comment();
    game.Play_Variation(index);
    Update_Move_List();
    Update_Move_List_Highlight();
    Update_Location();
    Update_Position_Info();
}

void
Game_Editor::Update_Board_Selection()
{
    if (game.Get_Variation_Depth() == 1 && game.Get_Current_Location() == 0)
        board.Clear_Last_Move();
    else
    {
        const Move & move = game.Get_Overall_Move
            (game.Get_Overall_Location() - 1);
        board.Set_Last_Move(move.from, move.to);
    }
}

void
Game_Editor::DO_Modified_Changed(bool modified)
{
    (void) modified;
}

void
Game_Editor::EW_Caption_Changed()
{
    notation_preferences.Caption_Changed();
    board.Caption_Changed();
}

void
Game_Editor::Update_Location()
{
}

void
Game_Editor::Update_Move_List()
{
    Update_Move_List_Length();
    Update_Move_List_Content();
    Update_Move_List_Highlight();
}

/**
 Dim undo and redo buttons
 */
void
Game_Editor::Dim(gboolean & undo_dimmed, gboolean & redo_dimmed)
{
    AppData *app_data = get_app_data();
    if (app_data != NULL && app_data->app_ui_data != NULL)
    {
        undo_dimmed = app_data->app_ui_data->undo_button->Is_Dimmed();
        redo_dimmed = app_data->app_ui_data->redo_button->Is_Dimmed();

        app_data->app_ui_data->undo_button->Dimmed(TRUE);
        app_data->app_ui_data->redo_button->Dimmed(TRUE);
    }

}

/**
 Undim undo and redo buttons
 */
void
Game_Editor::Undim(gboolean undo_dimmed, gboolean redo_dimmed)
{
    AppData *app_data = get_app_data();
    if (app_data != NULL && app_data->app_ui_data != NULL)
    {
        app_data->app_ui_data->undo_button->Dimmed(undo_dimmed);
        app_data->app_ui_data->redo_button->Dimmed(redo_dimmed);
    }
}

/**
 Get move from engine and play it
 */
void
Game_Editor::Engine_Play()
{
    /* If game is already ended don't wait for move from engine */
    if (stop_reading)
    {
        return;
    }


    gboolean undo_dimmed = FALSE, redo_dimmed = FALSE;

    Dim(undo_dimmed, redo_dimmed);

    Engine *engine = get_chess_engine();
    int computer = cc_b;
    if (!get_app_data()->app_ui_data->player_color_white)
        computer = cc_w;
    if (!engine->is_human() && computer == Get_Turn())
    {
        gchar *res = NULL;
        gboolean gotmove = FALSE;
        Set_Thinking();
        do
        {
            /* Run GTK main once if If GTK events pending, prevent freezing
             * UI */
            if (gtk_events_pending())
                gtk_main_iteration();

            /* Read from engine */
            if (res != NULL)
                g_free(res);

            res = engine->read();

            /* Game end? */
            if (gotmove && engine->game_end())
            {
                break;
            }
            if (continue_called && stop_reading)
            {
                stop_reading = FALSE;
                break;
            }
            else
            {
                if (res == NULL)
                {
                    continue;
                }
            }

            g_assert(res);

            gchar *pos = NULL;
            /* Handle the move reply from engine */
            if ((pos = strstr(res, ENGINE_REPLY_MOVE)) != NULL)
            {
                String tmp;
                pos += strlen(ENGINE_REPLY_MOVE) + 1;
                tmp = pos;

                int move = game.Get_Generator().Get_e2e4_Notation_Move(tmp);
                Play(move);
                gotmove = TRUE;
                // break;
            }
        } while (res != NULL || !gotmove || (res != NULL && gotmove));

        if (res == NULL)
        {
            osso_log(LOG_DEBUG, "Engine: error reading\n");
        }
        else
        {
            g_free(res);
        }

        /* End the game */
        if (engine->game_end())
        {
            chess_end(engine->get_result());
        }

        if (gtk_events_pending())
            gtk_main_iteration();


        Unset_Thinking();
    }

    Undim(undo_dimmed, redo_dimmed);
}

void
Game_Editor::Dragged()
{
    osso_log(LOG_DEBUG, "Game editor: Dragged");
    if (engine_thinking)
        return;

    int from, to;
    if (!board.Get_Squares(from, to))
        return;

    if (to == 0)                /* piece was dragged out of the board */
        return;

    int index = game.Get_Generator().Get_Square_Move(from, to);
    if (index >= 0)             /* play a move */
    {
        if (game.Get_Generator()[index].promotion.Is())
            /* move is a promotion */
        {
            if (!promotion_dialog)
            {
                AppData *app_data = get_app_data();
                promotion_dialog =
                    new Promotion_Dialog(app_data->app_ui_data->window);
            }
            int s = promotion_dialog->Get_Selected();
            if (s >= 0)
            {
                index += s;
            }
            else
            {
                return;
            }

        }
        Play(index);

        Engine_Play();

        return;
    }

    if (from != to)
    {
        /* no such legal move */
        sound_play(get_app_data()->app_sound_data, SOUND_MOVE_FAIL);
    }
    else
    {
        /* no actual move made */
        sound_play(get_app_data()->app_sound_data, SOUND_SELECT);
    }

    /* If it was the last move in the game, take it back.  Castling can also
     * be taken back by taking back the rook move. */

    if (game.Get_Overall_Location() == 0)
    {
        // Dbg("8");
        return;
    }

    const Move & last_move =
        game.Get_Overall_Move(game.Get_Overall_Location() - 1);
    if ((last_move.from == to && last_move.to == from)
        || (last_move.castling
            &&
            ((last_move.castling == Move::w_O_O && from == sq_f1
              && to == sq_h1) || (last_move.castling == Move::w_O_O_O
                                  && from == sq_d1 && to == sq_a1)
             || (last_move.castling == Move::b_O_O && from == sq_f8
                 && to == sq_h8) || (last_move.castling == Move::b_O_O_O
                                     && from == sq_d8 && to == sq_a8))))
    {
        Go_Backward();
        return;
    }

    /* take back last move and play another move with the same piece */
    if (last_move.to != from)
    {
        return;
    }

    /* determine index of last move.  -1 if a main variation or the index in
     * the variation list */
    int vindex = -1;
    if (!game.Go_Backward())
    {
        return;
    }
    if (game.Get_Current_Variation().
        Have_Variations(game.Get_Current_Location()))
        for (int i = 0;
             i <
             *game.Get_Current_Variation().Get_Variations(game.
                                                          Get_Current_Location
                                                          ()); ++i)
            if (game.Get_Current_Variation().
                Get_Variations(game.Get_Current_Location())[i] ()[0] ==
                last_move)
            {
                vindex = i;
                break;
            }
    for (int i = 0; i < *game.Get_Generator(); ++i)
    {
        Move new_move = game.Get_Generator()[i];
        if (new_move.from == last_move.from &&
            new_move.to != last_move.to && new_move.to == to)
        {
            board.Invalidate_Move_Squares(last_move);
            Play(i, false);
            board.Invalidate_Move_Squares(new_move);
            Update_Move_List_Length();
            Update_Move_List_Content(game.Get_Overall_Location() - 1);
            Update_Location();
            Update_Position_Info();
            // Modify();
            return;
        }
    }

    /* no such other move */

    if (vindex < 0)
    {

        game.Go_Forward();
    }
    else
    {
        game.Play_Variation(vindex);
    }
}

void
Game_Editor::Update_Variation_Depth()
{
    int depth = game.Get_Variation_Depth() - 1;
    assert(depth >= 0);
}
