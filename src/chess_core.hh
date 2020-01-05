/**
    @file chess_core.hh

    Prototypes for ChessCore

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

#ifndef CHESS_PAUSE_HH
#define CHESS_PAUSE_HH

#include <gtk/gtk.h>
#include "chess_communication.hh"
#include "chess_enginecommunication.hh"
#include "ui/interface.hh"
#include "settings.h"

/**
 Pauses the game.
 */
void chess_pause(GtkWidget *widget, gpointer data);

void chess_cursor_left(void);
void chess_cursor_right(void);
void chess_cursor_up(void);
void chess_cursor_down(void);
void chess_cursor_do(void);
/**
 Close the game.
 */
void chess_close(gboolean send_game_state);

/**
 End the game.
 */
void chess_end(gint result=0);

/**
  Show message for check.
*/
void chess_check();

/**
  Show message for check mate.
*/
void chess_check_mate();

/**
 Checks if focused window belongs to this application or not
*/
gboolean focus_is_own();

#endif
