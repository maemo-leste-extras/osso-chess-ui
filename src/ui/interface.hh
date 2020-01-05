/**
    @file interface.hh

    Function prototypes and variable definitions for general user
    interface functionality.

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

#ifndef INTERFACE_HH 
#define INTERFACE_HH 

/* GTK & GDK */
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include <libosso.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "chessui_i18n.hh"
#include "chess_core.hh"
#include "appdata.hh"

/* Babychess components */
#include "babychess/game_editor.hh"
#include "babychess/widget.hh"

/* Chess button */
#include "chess_button.hh"

/* Chess player status */
#include "chess_status.hh"

/* Include callback prototypes */
#include "callbacks.hh"


/* Definations */
#define BOARD_WIDTH 400
#define BOARD_HEIGHT -1

#define CTRL_WIDTH 220
#define CTRL_HEIGHT -1

#define CTRL_IMAGE "Background_control.jpg"

#define CTRL_UNDO_BUTTON_LEFT 68
#define CTRL_UNDO_BUTTON_TOP  390
#define CTRL_REDO_BUTTON_LEFT 174
#define CTRL_REDO_BUTTON_TOP  390

/* Overlay button */
#define CTRL_OVR_BUTTON_LEFT 	180
#define CTRL_OVR_BUTTON_TOP  	0
#define CTRL_OVR_BUTTON_H 	65
#define CTRL_OVR_BUTTON_W  	118

#define CTRL_STATUS_WHITE_LEFT 31
#define CTRL_STATUS_WHITE_TOP  312
#define CTRL_STATUS_BLACK_LEFT 31
#define CTRL_STATUS_BLACK_TOP  255

#define CTRL_MOVES_TOP  15
#define CTRL_MOVES_LEFT 38

#define CHESS_PLAYER1_NAME N_("game_fi_chess_player_1_title")
#define CHESS_PLAYER2_NAME N_("game_fi_chess_player_2_title")
#define CHESS_COMPUTER_NAME N_("game_fi_chess_computer_title")

#define MAX_MESSAGE 256

gboolean ui_main_window_hide(void *app_data);
gboolean ui_main_window_show(void *app_data);

gboolean dialog_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data);

/**
   Show or hide main view.

   @param show TRUE to show, FALSE to hide
 */
gboolean ui_view_main_window( AppData *app_data, gboolean show );

/**
   Create the main view. Initializes the application UI.

   @param app_data Pointer to AppData structure
*/
gboolean ui_create_main_window( void *app_data );

/**
   Create the main window.

   @return Pointer to GtkWindow
*/
GtkWindow *ui_create_main_view( void );

/**
   Create the chessboard.

   @param app_data Pointer to AppData structure
*/
void ui_create_chessboard(AppUIData *app_ui_data);

/**
   Create control area.

   @param app_data Pointer to AppData structure
*/
void ui_create_control(AppUIData *app_ui_data);

/**
   Show game end dialog and wait for reply.

   @param app_ui_data Pointer to AppUIData structure
   @param winner Name or type of winner.
 */
void ui_show_end_dialog(AppUIData *app_ui_data, gchar *winner);

/**
   Clean up UI, used before exiting program.

   @param app_ui_data Pointer to AppUIData structure
*/
void ui_clean_up( AppUIData *app_ui_data );


/**
  Show message for check.

  @param app_ui_data Pointer to AppUIData structure
*/
void ui_show_check_infoprint(AppUIData *app_ui_data);

/**
  Show message for check mate.

  @param app_ui_data Pointer to AppUIData structure
*/
void ui_show_check_mate_infoprint(AppUIData *app_ui_data);

/**
  Show player status.

  @param app_ui_data Pointer to AppUIData structure
*/
void ui_player_status(AppUIData *app_ui_data);

#endif
