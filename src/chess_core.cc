/**
    @file chess_core.cc

    Implements ChessCore

    Copyright (c) 2004, 2005 Nokia Corporation.
	
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA
*/

#include "chess_core.hh"
#include "sound.h"
#include "state_save.h"
#include <X11/Xlib.h>
#include <gdk/gdkx.h>

void chess_pause(GtkWidget *widget, gpointer)
{
  (void)widget;

  AppData *app_data = get_app_data();
  /* Ensure engine is paused */
  Engine *chess_engine = get_chess_engine();

  if (chess_engine) {
    chess_engine->force_move();
    /* Check for proper thinking time */
    if (chess_engine->enough_time()==FALSE) {
      /* Undo engine move */
      //chess_engine->undo();
      chess_engine->set_stopped(TRUE);
    }
  }

  if (app_data) {
    /* Simply save data, make some deinit, and exit as fast as possibly. Memory
    and other resource freeing will be done by the OS */
    save_state(app_data);
    sound_deinit(app_data->app_sound_data);
    deinit_engine();
    set_game_state(GAME_PAUSE_METHOD);
    deinit_osso(app_data);
    exit(0);
  }
  app_remove_timeout();
  gtk_main_quit();
}

void chess_cursor_left(void)
{
  AppData *app_data = get_app_data();
  if (app_data && app_data->app_ui_data && app_data->app_ui_data->game) {
    app_data->app_ui_data->game->Move_Cursor_Left();
  }
}
  
void chess_cursor_right(void) {
  AppData *app_data = get_app_data();
  if (app_data && app_data->app_ui_data && app_data->app_ui_data->game) {
    app_data->app_ui_data->game->Move_Cursor_Right();
  }
}
void chess_cursor_up(void) {
  AppData *app_data = get_app_data();
  if (app_data && app_data->app_ui_data && app_data->app_ui_data->game) {
    app_data->app_ui_data->game->Move_Cursor_Up();
  }
}
void chess_cursor_down(void) {
  AppData *app_data = get_app_data();
  if (app_data && app_data->app_ui_data && app_data->app_ui_data->game) {
    app_data->app_ui_data->game->Move_Cursor_Down();
  }
}
void chess_cursor_do(void) {
  AppData *app_data = get_app_data();
  if (app_data && app_data->app_ui_data && app_data->app_ui_data->game) {
    app_data->app_ui_data->game->Cursor_Action();
  }
}

void chess_close(gboolean send_game_state)
{
  if (send_game_state)
    set_game_state(GAME_CLOSE_METHOD);
    
  if (get_app_data())
  {
    sound_deinit(get_app_data()->app_sound_data);
  }
  
  app_remove_timeout();
  gtk_main_quit();
}

void chess_end(gint result)
{
    AppData *ad = get_app_data();
    /* Show dialog */
    if (ad!=NULL && result!=cr_none)
    {
        gchar *winner = NULL;
        if (cr_white == result)
        {
            if (ad->app_ui_data->player_color_white)
            {
                winner = _(CHESS_PLAYER1_NAME);
            }
            else
            {
                /* opponent won */
                if (ad->app_ui_data->opponent_type_human)
                {
                    winner = _(CHESS_PLAYER2_NAME);
                }
                else
                {
                    winner = _(CHESS_COMPUTER_NAME);
                }
            }
        }
        else if (cr_black == result) /* black won */
        {
            if (!ad->app_ui_data->player_color_white)
            {
                winner = _(CHESS_PLAYER1_NAME);
            }
            else
            {
                /* opponent won */
                if (ad->app_ui_data->opponent_type_human)
                {
                    winner = _(CHESS_PLAYER2_NAME);
                }
                else
                {
                    winner = _(CHESS_COMPUTER_NAME);
                }
            }
        }

        sound_play(ad->app_sound_data, SOUND_MATE);
        ui_show_end_dialog(ad->app_ui_data, winner);
    }

    /* Set game ended, send message to start up screen */
    //set_game_state(GAME_END_METHOD);
    //chess_close();
}

/* Show check infoprint */
void chess_check()
{
  AppData *ad = get_app_data();

  if (ad != NULL)
    ui_show_check_infoprint(ad->app_ui_data);
}

/* Show check mate infoprint */
void chess_check_mate()
{
  AppData *ad = get_app_data();
  
  if (ad != NULL)
    ui_show_check_mate_infoprint(ad->app_ui_data);
}


gboolean
focus_is_own()
{
    static Atom wm_class = XInternAtom(GDK_DISPLAY(), "WM_CLASS", true);

    Window focus_window;
    long BUFSIZE = 2048;
    int ret;
    Atom type_ret;
    int format_ret;
    unsigned long nitems_ret;
    unsigned long unused;
    unsigned char* data_ret = NULL;

    XGetInputFocus(GDK_DISPLAY(), &focus_window, &ret);
    if(PointerRoot == focus_window)
    {
        /* This value is returned, when clicking on titlebar of window, and
         * dialog becomes transparent, focus is in game main view */
        return TRUE;
    }

    if(None == focus_window)
    {
        return FALSE;
    }

    /* Getting WM_CLASS property of the window, who stole the focus */
    if (XGetWindowProperty(GDK_DISPLAY(), focus_window, wm_class, 
                           0L, BUFSIZE, False, AnyPropertyType,
                           &type_ret, &format_ret, &nitems_ret,
                           &unused, &data_ret ) == Success)
    {
	    if(!data_ret) {
		    AppData *app_data = get_app_data();
		    if(app_data->app_ui_data->dialog) {
			    return TRUE;
		    }
		    return FALSE;
	    }
        return ( (0 == strcmp((char *)data_ret,"chessui")) );
    }

    return FALSE;
}
