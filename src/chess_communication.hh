/**
    @file chess_communication.hh

    Prototypes for Communication

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

#ifndef CHESS_COMMUNICATION_HH
#define CHESS_COMMUNICATION_HH

#include <glib.h>
#include <libosso.h>

#include "chess_log.h"
#include "dbus.h"
#include "settings.h"
#include "chess_core.hh"
#include "chess_enginecommunication.hh"

#define STARTUP_SERVICE "com.nokia.osso_chess.startup"
#define STARTUP_IFACE   "com.nokia.osso_chess.startup"
#define STARTUP_OBJECT_PATH "/com/nokia/osso_chess/startup"

#define GAME_RUN_METHOD "game_run"
#define GAME_CONTINUE_METHOD "game_continue"
#define GAME_RESTART_METHOD "game_restart"
#define GAME_CLOSE_METHOD "game_close"
#define GAME_PAUSE_METHOD "game_pause"
#define GAME_HOME_METHOD "game_home"
#define GAME_END_METHOD "game_end"
#define GAME_SAVE_METHOD "game_save"
#define GAME_LOAD_METHOD "game_load"

#define CHESS_SAVE_FILE_TMP "/tmp/osso_chess.tmp"

/**
 Receive D-BUS messages and handles them

 @param interface The interface of the called method.
 @param method The method that was called.
 @param arguments A GArray of osso_rpc_t structures. 
 @param data An application specific pointer.
 @param retval The return value of the method.
 @returns osso_rpc_type_t value
 */
/*gint dbus_req_handler(const gchar *interface, const gchar *method,
  GArray *arguments, gpointer data, osso_rpc_t *retval);
  */

/**
 Determine board direction and update the view.
 */
void board_direction(AppData *app_data);

/**
 Handles start game method

 @returns osso_rpc_type_t value
 */
gboolean handle_start_game(void *notused);

/**
 Handles continue game method

 @returns osso_rpc_type_t value
 */
gint handle_continue_game(osso_rpc_t *retval);

/**
 Handles save game method

 @returns osso_rpc_type_t value
 */
gint handle_save_game(osso_rpc_t *retval);

/**
 Handles load game method

 @returns osso_rpc_type_t value
 */
gint handle_load_game(osso_rpc_t *retval);

/**
 Handles close game method

 @returns osso_rpc_type_t value
 */
gboolean handle_close_game(void *notused);

/**
 Handles incoming D-BUS message

 @param method The method that was called.
 @param arguments A GArray of osso_rpc_t structures. 
 @param data An application specific pointer.
 @param retval The return value of the method.
 @returns osso_rpc_type_t value
 */
gint handle_dbus_message(const gchar *method, GArray *arguments,
  gpointer data, osso_rpc_t *retval);

/**
 Send game state message to Start-up scren via D-BUS
 */
gboolean set_game_state(gchar *state);

/**
 Handles hardware events.

 @todo Just a skeleton
 @param state State occured.
 @param data Application specific data.
 */
void hw_event_handler(osso_hw_state_t *state, gpointer data);

/* Rama - Added to handle Display status */
void display_event_handler(osso_display_state_t state, gpointer data);

#endif
