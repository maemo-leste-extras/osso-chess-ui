/**
    @file state_save.c

    Application state saving.

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

#include <glib.h>
#include <libosso.h>
#include <string.h>
#include <unistd.h>
#include "state_save.h"
#include "settings.h"
#include "chess_enginecommunication.hh"
#include "ui/chess_moves_list.hh"

#define MAX_STRING 8
#define MAX_MOVES 100
#define DEFAULT_STATE_SAVE_FILE "/tmp/chess_state_file.pgn"

typedef struct {
  gboolean turn;
  gint pos;
  guint undo;
  /* TODO: What else do we need? */
} StateData;

void save_state(AppData *app_data)
{
  StateData statedata;
  osso_state_t state;
  
  if (!app_data || !app_data->app_ui_data || !app_data->app_ui_data->game ||
      !app_data->app_osso_data) {
    return;
  }

  /* Get the game engine */
  Engine *chess_engine = get_chess_engine();
  g_assert(chess_engine);

  if (chess_engine != NULL) {
    /* Get the location from GConf */
    gchar *filename = settings_get_string(
                      SETTINGS_STATE_SAVE_FILE);
    gboolean free_filename = TRUE;

    if (filename == NULL) {
      filename = DEFAULT_STATE_SAVE_FILE;
      settings_set_string(
        SETTINGS_STATE_SAVE_FILE, filename);
      free_filename = FALSE;
    } else {
      free_filename = TRUE;
    }

    if (filename!=NULL) {
      /* Save file to specified location */
      AppData *app_data = get_app_data();
      if (app_data->app_ui_data->game->Save(filename)) {
        /* TODO: Check? */
      }
      
      if (free_filename == TRUE) {
        g_free(filename);
      }
    }
  }

  statedata.pos = app_data->app_ui_data->game->Get_Cursor_Pos();

  if (app_data->app_ui_data->white_status->Get_Active()) {
    statedata.turn = TRUE;
  } else {
    statedata.turn = FALSE;
  }

  statedata.undo = app_data->app_ui_data->game->Get_Location();
  
  state.state_data = (void *)&statedata;
  state.state_size = sizeof(statedata);
  if ((app_data->app_ui_data->decrease_undo==TRUE)||(chess_engine->get_result()==cr_white)||(chess_engine->get_result()==cr_black)||(app_data->app_ui_data->mate_pos))
  {
    statedata.undo--;
  }
  osso_state_write(app_data->app_osso_data->osso, &state);  
}

void read_state(AppData *app_data)
{
  StateData statedata;
  osso_state_t state;
  int i;

  if (!app_data || !app_data->app_osso_data ||
      !app_data->app_ui_data || !app_data->app_ui_data->game) return;

  state.state_data = (void *)&statedata;
  state.state_size = sizeof(statedata);
  
  if (osso_state_read(app_data->app_osso_data->osso, &state) != OSSO_OK)
    return;

  /* Get the game engine */
  Engine *chess_engine = get_chess_engine();
  g_assert(chess_engine);

  if (chess_engine != NULL) {
    /* Get the location from GConf */
    gchar *filename = settings_get_string(
                      SETTINGS_STATE_SAVE_FILE);
    /* Load file from specified location */
    if (filename!=NULL && chess_engine->load(filename)) {
      /* Load UI part */
      app_data->app_ui_data->game->New();

      if (! app_data->app_ui_data->game->
        Load(filename)) { 
	     /* TODO: Check? */
	   }
    }
    if (filename!=NULL) {
      g_free(filename);
    }
  }

  app_data->app_ui_data->game->Cursor_Pos(statedata.pos);

  i = statedata.undo;
  while (i<(app_data->app_ui_data->game->Get_Location()))
    app_data->app_ui_data->game->Go_Backward();

}
