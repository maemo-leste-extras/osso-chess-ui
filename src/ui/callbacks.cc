/**
    @file callbacks.c

    Provides callbacks for the user interface.

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

/* Function prototypes */
#include "callbacks.hh"
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <X11/Xlib.h>
#include <gdk/gdkx.h>

/* Definitions of UI identificators and AppData */
#include "interface.hh"

#define KEY_ESCAPE_TIMEOUT 1500
guint escape_timeout = 0;

gboolean is_key_press = FALSE;
gboolean is_back_key_press = FALSE;

gboolean working=TRUE;

gboolean main_win_focus_out(GtkWidget *,
                            GdkEventFocus *,
                            AppUIData *)
{
    if (!focus_is_own())
    {
        chess_pause(NULL,NULL);
    }

    return  FALSE;
}


gboolean delete_window_cb(GtkWidget *, GdkEvent *,
                          AppUIData *)
{
    chess_pause(NULL,NULL);
    return TRUE;
}


gboolean work_done(gpointer)
{
    working=FALSE;
    return working;
}

gboolean key_press(GtkWidget *widget, GdkEventKey *event, 
        AppUIData *app_ui_data)
{
    (void)widget;
    /*if (event->state & (GDK_CONTROL_MASK |
      GDK_SHIFT_MASK |
      GDK_MOD1_MASK |
      GDK_MOD3_MASK |
      GDK_MOD4_MASK |
      GDK_MOD5_MASK)) {
      return TRUE;
      }

      osso_log(LOG_DEBUG,"Key: %d\n",event->keyval);
      */
    if (!event) return TRUE;

    is_key_press = TRUE;
    if (!working)
    {
        switch (event->keyval) {
            case GDK_Escape:
                if (escape_timeout == 0) {
                    escape_timeout = g_timeout_add(KEY_ESCAPE_TIMEOUT, 
                            app_escape_timeout, (gpointer)app_ui_data);
                }
                return TRUE;
            case GDK_Left:
                chess_cursor_left();
                break;
            //case GDK_KP_Enter:
            case GDK_F4:
            case GDK_F5:
            case GDK_F6:
            case GDK_F10:
                chess_pause(widget, app_ui_data);
                is_key_press = FALSE;

                return TRUE;
            case GDK_Right:
                chess_cursor_right();
                break;
            case GDK_Up:
                chess_cursor_up();
                break;
            case GDK_Down:
                chess_cursor_down();
                break;
            case GDK_Return:
            case GDK_KP_Enter:
                chess_cursor_do();
                break;
            default:
                osso_log(LOG_DEBUG,"Key: %d\n",event->keyval);
                break;
        }
    }
    return FALSE;
}

gboolean key_release(GtkWidget *widget, GdkEventKey *event, 
        AppUIData *app_ui_data)
{
    /*if (event->state & (GDK_CONTROL_MASK |
      GDK_SHIFT_MASK |
      GDK_MOD1_MASK |
      GDK_MOD3_MASK |
      GDK_MOD4_MASK |
      GDK_MOD5_MASK)) {
      return TRUE;
      }

      osso_log(LOG_DEBUG,"Key: %d\n",event->keyval);
      */

    if (!event) return TRUE;


    switch (event->keyval) {
        case GDK_Escape:
            app_remove_timeout();
            if (is_key_press)
                chess_pause(widget, app_ui_data);
            is_key_press = FALSE;

            return TRUE;



        case GDK_minus:
        case GDK_KP_Subtract:
        case GDK_plus:
        case GDK_KP_Add:
        default:
            break;
    }

    is_key_press = FALSE;

    return FALSE;
}

gboolean visibility_notify_event( GtkWidget *widget, GdkEventVisibility *event,
        AppUIData *app_ui_data)
{
    switch (event->state) {
        case GDK_VISIBILITY_FULLY_OBSCURED:
            chess_pause(widget, app_ui_data);
            break;
        case GDK_VISIBILITY_UNOBSCURED:
        case GDK_VISIBILITY_PARTIAL:
            break;
    }

    return FALSE;
}


gboolean app_escape_timeout(gpointer data)
{
    AppUIData *app_ui_data = (AppUIData *)data;

    if (!app_ui_data)
        return TRUE;

    settings_set_bool(SETTINGS_ESCAPE_TIMEOUT, TRUE);

    escape_timeout = 0;
    chess_pause(NULL, app_ui_data);

    return FALSE;
}

void app_remove_timeout(void)
{
    if (escape_timeout > 0) {
        g_source_remove(escape_timeout);
        escape_timeout = 0;
    }
}

gboolean on_overlay_expose(GtkWidget *widget, GdkEventExpose * event, AppUIData *data)
{
	AppUIData *app_ui_data = (AppUIData *)data;
	GdkPixmap *pixmap;
	GdkBitmap *bitmask;

	GdkColormap *colormap = gdk_screen_get_rgba_colormap(gtk_widget_get_screen(
				   GTK_WIDGET(app_ui_data->overlay_button)));

	g_assert(colormap);
	gdk_drawable_set_colormap(GTK_WIDGET(app_ui_data->overlay_button)->window,
					colormap);
	gdk_pixbuf_render_pixmap_and_mask_for_colormap (app_ui_data->overlay_back,
						colormap,
						&pixmap, &bitmask,
						127);
	if(pixmap)
	{
	g_assert(pixmap);
		gdk_window_set_back_pixmap(GTK_WIDGET(app_ui_data->overlay_button)->window,
				pixmap, FALSE); 
		g_object_unref(pixmap);
	}
	if(bitmask)
	{	
		 gdk_window_shape_combine_mask(GTK_WIDGET(app_ui_data->overlay_button)->window,
				 bitmask, 0, 0);
		 g_object_unref(bitmask);
	}
	gdk_draw_pixbuf(GTK_WIDGET(app_ui_data->overlay_button)->window,
	                GTK_WIDGET(app_ui_data->overlay_button)->style->fg_gc[GTK_STATE_NORMAL],
	                app_ui_data->overlay_back, 0, 0,
	                CTRL_OVR_BUTTON_LEFT, CTRL_OVR_BUTTON_TOP,
	                CTRL_OVR_BUTTON_W, CTRL_OVR_BUTTON_H,
			GDK_RGB_DITHER_NONE, 0, 0);
	return TRUE;
}	

gboolean on_back_button_press(GtkWidget *widget, GdkEventButton *event, AppUIData * data)
{
	GtkRequisition req;
	AppUIData *app_ui_data = (AppUIData *)data;

	gtk_widget_size_request(app_ui_data->control_image, &req);
	if(((gint)event->x > (BOARD_WIDTH + req.width - CTRL_OVR_BUTTON_W)) &&
			((gint)event->y < CTRL_OVR_BUTTON_H)) {
		is_back_key_press = TRUE;
	}
	return TRUE;	
}

gboolean on_back_button_release(GtkWidget *widget, GdkEventButton *event, AppUIData * data)
{
	GtkRequisition req;
	AppUIData *app_ui_data = (AppUIData *)data;

	gtk_widget_size_request(app_ui_data->control_image, &req);

	if(((gint)event->x > (BOARD_WIDTH + req.width - CTRL_OVR_BUTTON_W)) &&
			((gint)event->y < CTRL_OVR_BUTTON_H) && is_back_key_press)
	{
		chess_pause(widget, data);
		is_back_key_press = FALSE;
	}

	return TRUE;
}
