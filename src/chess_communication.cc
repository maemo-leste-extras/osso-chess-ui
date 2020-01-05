/**
    @file chess_communication.cc

    Implements Communication module

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

#include <sys/stat.h>
#include <libgnomevfs/gnome-vfs.h>
#include "chess_communication.hh"
#include "sound.h"
#include "state_save.h"

gboolean continue_called=FALSE;
gboolean CLOSE_CALLED = FALSE;



gboolean handle_close_game(void *) {
  osso_log(LOG_DEBUG,"Closing game");
  /* Close the game */
  chess_close(TRUE);
  return FALSE;
}
void board_direction(AppData *app_data) {

  if (!app_data || !app_data->app_ui_data || !app_data->app_ui_data->game) {
    return;
  }

  if (app_data->app_ui_data->player_color_white) {
    app_data->app_ui_data->game->Turn_Board(false);
  }
  else {
    app_data->app_ui_data->game->Turn_Board(true);
  }
}

gboolean handle_start_game(void *) {
  /* Start a new game */
  deinit_engine();
  Engine *chess_engine = get_chess_engine();
  g_assert(chess_engine);

  if (chess_engine != NULL) {
    AppData *app_data = get_app_data();
    app_data->app_ui_data->game->New();

    /* Command engine to start */
    chess_engine->new_game();
    set_engine_settings(chess_engine);

    /* Tutor mode */
    gboolean tutor = app_data->app_ui_data->show_legal_moves;
    app_data->app_ui_data->game->Tutor_Mode(tutor);

    if (app_data->app_ui_data->enable_sound)
      sound_init(app_data->app_sound_data);
    else
      sound_deinit(app_data->app_sound_data);

    board_direction(app_data);

    /* Show main window */
    ui_view_main_window(app_data,TRUE);

    app_data->app_ui_data->game->Update_To_Move();
  
    if (!app_data->app_ui_data->player_color_white &&
        !app_data->app_ui_data->opponent_type_human) {
      chess_engine->go();
      app_data->app_ui_data->game->Start_Engine();
    }

    /* Set return value */
  }
  return FALSE;
}


gint handle_continue_game(osso_rpc_t *retval) {
  osso_log(LOG_DEBUG,"Continuing game");
  continue_called=TRUE;

  /* Continue the game */
  deinit_engine();
  Engine *chess_engine = get_chess_engine();
  g_assert(chess_engine);

  if (chess_engine != NULL) {
    AppData *app_data = get_app_data();

    //read_state(app_data);

    osso_log(LOG_DEBUG,"Continue: Chess engine ok");

    /* Check if enough thinking time before stopped */
    /* XXX: should always be in ready state
    if (chess_engine->get_stopped()==TRUE) {
      chess_engine->set_stopped(FALSE);
    }
    */

    set_engine_settings(chess_engine);
    read_state(app_data);

    /* Tutor mode (show legal moves) */
    gboolean tutor = app_data->app_ui_data->show_legal_moves;
    app_data->app_ui_data->game->Tutor_Mode(tutor);

    if (app_data->app_ui_data->enable_sound)
      sound_init(app_data->app_sound_data);
    else
      sound_deinit(app_data->app_sound_data);

    board_direction(app_data);

    /* Show main window */
    ui_view_main_window(app_data,TRUE);

    app_data->app_ui_data->game->Update_To_Move();
  
    /*
    gboolean turn = TRUE;

    if (app_data->app_ui_data->white_status->Get_Active())
      turn = TRUE;
    else turn = FALSE;
    
    if (!app_data->app_ui_data->opponent_type_human) {
      gboolean run = FALSE;
      if (!app_data->app_ui_data->player_color_white && turn) {
        run = TRUE;
      }
      if (app_data->app_ui_data->player_color_white && !turn) {
        run = TRUE;
      }

      if (run) {
        chess_engine->go();
        app_data->app_ui_data->game->Start_Engine();
      }

    }
    */
    
    /* Check and start engine in case of Human vs Device */
    if (!app_data->app_ui_data->opponent_type_human) {
      /* white_status is always Human, this is not the color indicator */
      if (!app_data->app_ui_data->white_status->Get_Active()) {
        chess_engine->go();
        app_data->app_ui_data->game->Start_Engine();
      }
    }
            
    /* Set return value */
    g_assert(retval);
    retval->type=DBUS_TYPE_BOOLEAN;
    retval->value.b=TRUE;
    
    
  } else {
    osso_log(LOG_ERR,"Continue: Chess engine failed");

    /* Set return value */
    g_assert(retval);
    retval->type=DBUS_TYPE_BOOLEAN;
    retval->value.b=FALSE;
  }
  continue_called=FALSE;
  return OSSO_OK;
}

gboolean check_dir(gchar *filename)
{
  if (filename==NULL) return FALSE;
  gchar *dirn = g_strdup(filename);
  if (dirn==NULL) return FALSE;

  gchar *pos = g_strrstr(dirn,"/");
  if (pos!=NULL)
    *pos=0;

  if (chdir(dirn)<0) {
    if (mkdir(dirn,0755)<0) {
      g_free(dirn);
      return FALSE;
    }
  }

  g_free(dirn);
  return TRUE;
}

gint handle_save_game(osso_rpc_t *retval)
{
  osso_log(LOG_DEBUG,"Saving game");
  
  if (!retval) return OSSO_INVALID;

  /* Get the game engine */
  Engine *chess_engine = get_chess_engine();
  g_assert(chess_engine);

  if (chess_engine != NULL) {
      g_assert(retval);

      AppData *app_data = get_app_data();

      read_state(app_data);

      /* Save file to specified location */
      /* if (chess_engine->save(filename)) { */
      if (app_data->app_ui_data->game->Save(CHESS_SAVE_FILE_TMP)) {

        retval->type=DBUS_TYPE_BOOLEAN;
        retval->value.b=TRUE;        
      } else {
        retval->type=DBUS_TYPE_BOOLEAN;
        retval->value.b=FALSE;
      }

      save_state(app_data);

  } else {
    /* Set return value */
    g_assert(retval);
    retval->type=DBUS_TYPE_BOOLEAN;
    retval->value.b=FALSE;
  }

  /* Release xboard */
  chess_close(FALSE);
  
  return OSSO_OK;
}

gint handle_load_game(osso_rpc_t *retval)
{
    int filedesc=0;
    osso_log(LOG_DEBUG,"Loading game");

    if (!retval) return OSSO_INVALID;

    /* Continue the game */
    Engine *chess_engine = get_chess_engine();
    g_assert(chess_engine);

    if (chess_engine != NULL) {
        AppData *app_data = get_app_data();

        /* Get the location from GConf */
        gchar *filename = settings_get_string(SETTINGS_SAVE_FILE);
        //app_data->app_ui_data->save_file;
        
        if (filename) {
        
        	GnomeVFSResult vfs_res;

            gchar *temp = g_strdup("/tmp/chessXXXXXX");
            filedesc=mkstemp(temp);
            if (filedesc!=-1)
            {
                close(filedesc);
            }
            else{
            	g_free( temp );
                retval->type=DBUS_TYPE_BOOLEAN;
                retval->value.b=FALSE;
                return OSSO_OK;
            }
            gchar *tmp_uristr = gnome_vfs_get_uri_from_local_path(temp);
            GnomeVFSURI *tmp_uri = gnome_vfs_uri_new(tmp_uristr),
                        *file_uri = gnome_vfs_uri_new(filename);
            g_free(tmp_uristr);
            vfs_res = gnome_vfs_xfer_uri(file_uri, tmp_uri,
                    GNOME_VFS_XFER_DEFAULT,
                    GNOME_VFS_XFER_ERROR_MODE_ABORT,
                    GNOME_VFS_XFER_OVERWRITE_MODE_REPLACE,
                    NULL, NULL);                    

            gnome_vfs_uri_unref(tmp_uri);
            gnome_vfs_uri_unref(file_uri);

            if (vfs_res == GNOME_VFS_OK && chess_engine->load(temp)) {
                app_data->app_ui_data->game_loading = TRUE;

                /* Load UI part */
                app_data->app_ui_data->game->New();

                if (! app_data->app_ui_data->game->
                        Load(temp)) {
                    /* Release xboard */
                    chess_close(FALSE);
                    unlink(temp);
                    g_free(temp);

                    app_data->app_ui_data->game_loading = FALSE;

                    retval->type=DBUS_TYPE_BOOLEAN;
                    retval->value.b=FALSE;
                    return OSSO_OK;
                } 
                app_data->app_ui_data->game_loading = FALSE;

                osso_log(LOG_DEBUG, "Game loaded from %s\n",
                        filename);

                /* Update player status*/
                ui_player_status(app_data->app_ui_data);

                /* Update game setting to engine */
                set_engine_settings(chess_engine);

                /* Tutor mode */
                gboolean tutor = app_data->app_ui_data->show_legal_moves;
                app_data->app_ui_data->game->Tutor_Mode(tutor);

                if (app_data->app_ui_data->enable_sound)
                    sound_init(app_data->app_sound_data);
                else
                    sound_deinit(app_data->app_sound_data);

                board_direction(app_data);

                /* Show main window */
                ui_view_main_window(app_data, TRUE);

                app_data->app_ui_data->game->Update_To_Move();

                /* Check and start engine in case of Human vs Device */
                if (!app_data->app_ui_data->opponent_type_human) {
                    /* white_status is always Human,
                     * this is not the color indicator */
                    if (!app_data->app_ui_data->white_status->Get_Active()) {
                        chess_engine->go();
                        app_data->app_ui_data->game->Start_Engine();
                    }
                }

                /* g_free(filename); */

                retval->type=DBUS_TYPE_BOOLEAN;
                retval->value.b=TRUE;
            } else {
            
            	if( vfs_res != GNOME_VFS_OK ) {
               		g_warning( "Error '%s' while copying '%s' to '%s'",
               			gnome_vfs_result_to_string( vfs_res ),
               			filename, temp );
               	} else {
               		g_warning( "File '%s' not accepted by engine", filename );               	
               	}
                retval->type=DBUS_TYPE_BOOLEAN;
                retval->value.b=FALSE;
                chess_close(FALSE);
            }

            g_free(filename);
            unlink(temp);
            g_free(temp);
        } else {
           	g_warning( "Missing file name" );
            /* Set return value */
            g_assert(retval);
            retval->type=DBUS_TYPE_BOOLEAN;
            retval->value.b=FALSE;
        }

    } else {
    	g_critical( "Missing engine" );
        g_assert(retval);
        retval->type=DBUS_TYPE_BOOLEAN;
        retval->value.b=FALSE;
    }
    return OSSO_OK;
}

gint handle_dbus_message(const gchar *method, GArray *arguments,
  gpointer data, osso_rpc_t *retval)
{
  (void) arguments;
  (void) data;
  g_assert(method);
  
  if (!retval || !method) return OSSO_INVALID;

  osso_log(LOG_DEBUG,"DBUS method in chess: %s",method);

  AppData *app_data = get_app_data();
  app_data->app_ui_data->game_loading = FALSE;

  /* Handle start game and restart */
  if (g_ascii_strcasecmp(method,GAME_RUN_METHOD)==0 ||
      g_ascii_strcasecmp(method,GAME_RESTART_METHOD)==0) {
        g_idle_add(handle_start_game,NULL);
        retval->type=DBUS_TYPE_BOOLEAN;
        retval->value.b=TRUE;
        return OSSO_OK;
  }
  /* Handle continue game */
  else if (g_ascii_strcasecmp(method,GAME_CONTINUE_METHOD)==0) {

    return handle_continue_game(retval);
  }
  /* Handle save game */
  else if (g_ascii_strcasecmp(method,GAME_SAVE_METHOD)==0) {

    return handle_save_game(retval);
  }
  /* Handle load game */
  else if (g_ascii_strcasecmp(method,GAME_LOAD_METHOD)==0) {

    return handle_load_game(retval);
  }
  /* Handle close game */
  else if (g_ascii_strcasecmp(method,GAME_CLOSE_METHOD)==0) {
    CLOSE_CALLED = TRUE;
    g_idle_add_full(G_PRIORITY_HIGH,handle_close_game,NULL,NULL);
    retval->type=DBUS_TYPE_BOOLEAN;
    retval->value.b=TRUE;
    return OSSO_OK;
  }
  else {
    osso_log(LOG_ERR,"Unknown DBUS method: %s", method);

    retval->type=DBUS_TYPE_INVALID;
    retval->value.s="Unknown method";
  }

  return OSSO_ERROR;
}

gboolean set_game_state(gchar* method)
{
  if (!method) return FALSE;

  osso_return_t ret;
  osso_rpc_t retval;

  osso_log(LOG_DEBUG,"Writing to DBUS: %s\n",method);

  /* Send message */
  ret = send_dbus_message(
    STARTUP_SERVICE,
    STARTUP_OBJECT_PATH,
    STARTUP_IFACE,
    method,
    NULL,
    &retval
  );

  osso_log(LOG_DEBUG,"Return value type: %d\n",retval.type);
	osso_rpc_free_val (&retval);

  if (ret == OSSO_OK) return TRUE;
  return FALSE;
}

void hw_event_handler(osso_hw_state_t *state, gpointer data)
{
  (void) data;

  if (!state) return;

  if (state->shutdown_ind) {
    /* Rebooting */
    chess_close(FALSE);
  }
  if (state->memory_low_ind) {
    /* Memory low */
    chess_close(TRUE);
  }
  if (state->save_unsaved_data_ind) {
    /* Unsaved data should be saved */
    if (data) {
      chess_pause(NULL, ((AppData *)data)->app_ui_data);
    }
  }
  if (state->system_inactivity_ind) {
    /* Minimize activity */
    Engine *ce = get_chess_engine();

    /* Stop computer thinking */
    ce->force_move();
    /* Rama - NB#96276. call puase handler when 
     * backscreen goes to sleep */
    if (data) {
      chess_pause(NULL, ((AppData *)data)->app_ui_data);
    }
  }
}


void display_event_handler(osso_display_state_t state, gpointer data)
{
	
	if(OSSO_DISPLAY_ON == state) {
		/* Do Nothing */
	}
	else if( (OSSO_DISPLAY_OFF == state) || (OSSO_DISPLAY_DIMMED == state) ) {
		if (data) {
			Engine *ce = get_chess_engine();

			/* Stop computer thinking */
			ce->force_move();
    
			chess_pause(NULL, ((AppData *)data)->app_ui_data);
		}
	}

	return;
}
