/**
    @file callbacks.h

    Function definitions for user interface callbacks.

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

/* GTK */
#include <gtk/gtk.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "appdata.hh"
#include "chess_core.hh"
#include "chessui_i18n.hh"

gboolean    main_win_focus_out(GtkWidget          *widget,
                                            GdkEventFocus *event,
                                            AppUIData *app_ui_data);

/* Called, when the window is deleted; sends game_pause to games startup */
gboolean delete_window_cb(GtkWidget *widget, GdkEvent *event,
                          AppUIData *app_ui_data);


gboolean work_done(gpointer data);

/**
 Handle key press event
 @param widget Widget receiving the event.
 @param event The event.
 @param app_ui_data Application data.
 @return TRUE if modifier pressed or known event, FALSE otherwise.
*/
gboolean key_press(GtkWidget *widget, GdkEventKey *event, 
		   AppUIData *app_ui_data);

/**
 Handle key release event
 @param widget Widget receiving the event.
 @param event The event.
 @param app_ui_data Application data.
 @return TRUE if modifier pressed or known event, FALSE otherwise.
*/
gboolean key_release(GtkWidget *widget, GdkEventKey *event, 
		   AppUIData *app_ui_data);

/**
 Handle key escape timeout event
 @param app_ui_data Application data.
 @return It should return FALSE if the source should be removed..
*/
gboolean app_escape_timeout(gpointer data);

/**
 Handle visibility notify event
 @param widget Widget receiving the event.
 @param event The event.
 @param app_ui_data Application data.
 @return TRUE to stop other handlers, FALSE to propagate the event further.
*/
gboolean visibility_notify_event( GtkWidget *widget, GdkEventVisibility *event,
                   AppUIData *app_ui_data);


/**
 Handle remove timeout
*/ 
void app_remove_timeout(void);

gboolean on_overlay_expose(GtkWidget *widget, GdkEventExpose *event, AppUIData * data);
gboolean on_back_button_press(GtkWidget *widget, GdkEventButton *event, AppUIData * data);
gboolean on_back_button_release(GtkWidget *widget, GdkEventButton *event, AppUIData * data);
