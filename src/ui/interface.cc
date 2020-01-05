/**
    @file interface.cc

    Implementation of the user interface functions

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

/* Include function prototypes */
#include "interface.hh"
#include <gtk/gtk.h>
#include <hildon/hildon-banner.h>
#include <gdk/gdkkeysyms.h>
#include <hildon/hildon.h>


gulong expose_id=0;


gboolean ui_main_window_show(void *app_data) {
    AppUIData *app_ui_data = ((AppData *)app_data)->app_ui_data;
    osso_log(LOG_DEBUG,"Showing window and setting full screen");
    if (app_ui_data == NULL) {
        return TRUE;
    }

    gtk_window_fullscreen( app_ui_data->window );
    gtk_widget_show_all( GTK_WIDGET (app_ui_data->window) );
    return FALSE;
}


gboolean ui_main_window_hide(void *app_data) {
    AppUIData *app_ui_data =  ((AppData *)app_data)->app_ui_data;
    osso_log(LOG_DEBUG,"Hiding window");
    if (app_ui_data == NULL) {
        return TRUE;
    }

    gtk_widget_hide_all( GTK_WIDGET (app_ui_data->window) );
    gtk_window_unfullscreen( app_ui_data->window );
    return FALSE;
}

/* Show or hide main view */
gboolean ui_view_main_window( AppData *app_data, gboolean show )
{
  if (!app_data || !app_data->app_ui_data) return FALSE;
  if (show == TRUE) {
      ui_main_window_show((void *) app_data);
  } else {
      ui_main_window_hide((void *) app_data);
  }
  return FALSE; /* FALSE because false removes the function from idle loop */
}

gboolean main_win_set_callbacks(gpointer data);

gboolean main_win_set_callbacks(gpointer data)
{
  AppUIData *app_ui_data=(AppUIData*)data;
  gtk_window_present(app_ui_data->window);
  gtk_widget_add_events(GTK_WIDGET(app_ui_data->window), GDK_VISIBILITY_NOTIFY_MASK);
  g_signal_connect (G_OBJECT (app_ui_data->window), "visibility-notify-event",
      G_CALLBACK (visibility_notify_event), app_ui_data);
  g_signal_connect(GTK_WIDGET(app_ui_data->window),"focus-out-event",G_CALLBACK(main_win_focus_out), app_ui_data);
  return FALSE;
}

gboolean main_win_expose(GtkWidget      *widget,
                                            GdkEventExpose *event,
                                            gpointer        user_data);

gboolean main_win_expose(GtkWidget      *,
                                            GdkEventExpose *,
                                            gpointer        user_data)
{
  AppUIData *app_ui_data=(AppUIData*)user_data;
  gtk_window_present(app_ui_data->window);
  g_signal_handler_block(app_ui_data->window,expose_id);
  g_timeout_add(300,main_win_set_callbacks,user_data);
  return FALSE;
}

/* Create the main view */
gboolean ui_create_main_window( void *app_data )
{
  if (!app_data || ! ((AppData *)app_data)->app_ui_data) return TRUE;

  AppUIData *app_ui_data = ((AppData *)app_data)->app_ui_data;
  g_assert(app_ui_data);

  /* Init GdkRGB. */
  /* gdk_rgb_init(); */

  app_ui_data->window = ui_create_main_view();
  expose_id=g_signal_connect(app_ui_data->window,"expose-event",G_CALLBACK(main_win_expose),app_ui_data);
  /* Create a hbox for all our stuff inside main view */
  app_ui_data->main_hbox = gtk_hbox_new( FALSE, 0 );
  gtk_container_add( GTK_CONTAINER (app_ui_data->window),
		     app_ui_data->main_hbox );

   gtk_widget_add_events(GTK_WIDGET(app_ui_data->window),
		GDK_POINTER_MOTION_MASK |
		GDK_BUTTON_MOTION_MASK |
		GDK_BUTTON_PRESS_MASK |
		GDK_BUTTON_RELEASE_MASK);
  /* Singal connect */
  g_signal_connect (G_OBJECT (app_ui_data->window), "key-press-event",
      G_CALLBACK (key_press), app_ui_data);
      
  g_signal_connect (G_OBJECT (app_ui_data->window), "key-release-event",
      G_CALLBACK (key_release), app_ui_data);

  g_signal_connect(G_OBJECT(app_ui_data->window), "delete-event",
                   G_CALLBACK(delete_window_cb), app_ui_data);

  g_signal_connect(G_OBJECT(app_ui_data->window), "button_press_event",
                   G_CALLBACK(on_back_button_press), app_ui_data);
  g_signal_connect(GTK_WIDGET(app_ui_data->window), "button_release_event",
		   G_CALLBACK(on_back_button_release), app_ui_data);	

  ui_create_chessboard(app_ui_data);

  ui_create_control(app_ui_data);
  g_idle_add(work_done,NULL);
  ui_main_window_hide(app_data);
  return FALSE;

}

void ui_create_chessboard(AppUIData *app_ui_data)
{
  if (!app_ui_data) return;

  /* Assign hbox to BabyChess Widget */
  osso_log(LOG_DEBUG,"Creating Widget for board");
  app_ui_data->bbox = new Widget();
  app_ui_data->bbox->Attach(app_ui_data->main_hbox,false);

  /* Make game editor */
  osso_log(LOG_DEBUG,"Creating game editor and board");
  app_ui_data->game = new Game_Editor();
  app_ui_data->game->Open(app_ui_data->bbox);
  osso_log(LOG_DEBUG,"Setting game editor size");
  app_ui_data->game->Set_Size(BOARD_WIDTH, BOARD_HEIGHT);

}

void ui_create_control(AppUIData *app_ui_data)
{
  if (!app_ui_data) return;

  /* Fixed for control */
  osso_log(LOG_DEBUG,"Creating control area");
  app_ui_data->control = gtk_fixed_new();
  gtk_widget_set_size_request(app_ui_data->control,
  	CTRL_WIDTH, CTRL_HEIGHT);
  gtk_container_add( GTK_CONTAINER (app_ui_data->main_hbox),
  		app_ui_data->control );

  /* Background image */
  osso_log(LOG_DEBUG,"Setting background image");
  app_ui_data->control_image = gtk_image_new_from_file(
  	PIXMAPSDIR "/" CTRL_IMAGE );

  gtk_fixed_put(GTK_FIXED(app_ui_data->control),
  	app_ui_data->control_image, 0, 0);

  /* For button and status positioning */
  GtkWidget *tmp1;
  Widget *tmp2;

  /* Undo button: Set a hbox to wanted position */
  osso_log(LOG_DEBUG,"Undo button: Position");
  tmp1 = gtk_hbox_new(TRUE, 0);
  gtk_fixed_put(GTK_FIXED(app_ui_data->control), tmp1,
  	CTRL_UNDO_BUTTON_LEFT, CTRL_UNDO_BUTTON_TOP);

  /* Undo button: Make BabyChess Widget of the hbox, set expose events */
  osso_log(LOG_DEBUG,"Undo button: Babychess workaround");
  tmp2 = new Widget();
  tmp2->Attach(tmp1, true);

  /* Undo button: Put the Undo button inside the box Widget */
  osso_log(LOG_DEBUG,"Undo button: Creating the button");
  app_ui_data->undo_button = new Chess_Button();
  app_ui_data->undo_button->Load(cb_undo);
  app_ui_data->undo_button->Open(tmp2);
  app_ui_data->undo_button->Dimmed(TRUE);

  /* Redo button (as Undo button) */
  osso_log(LOG_DEBUG,"Redo button: Position");
  tmp1 = gtk_hbox_new(TRUE, 0);
  gtk_fixed_put(GTK_FIXED(app_ui_data->control), tmp1,
  	CTRL_REDO_BUTTON_LEFT, CTRL_REDO_BUTTON_TOP);

  osso_log(LOG_DEBUG,"Redo button: Babychess workaround");
  tmp2 = new Widget();
  tmp2->Attach(tmp1, true);

  osso_log(LOG_DEBUG,"Redo button: Creating the button");
  app_ui_data->redo_button = new Chess_Button();
  app_ui_data->redo_button->Load(cb_redo);
  app_ui_data->redo_button->Open(tmp2);
  app_ui_data->redo_button->Dimmed(TRUE);

  /* White and Black status */
  osso_log(LOG_DEBUG,"White player: Position");
  tmp1 = gtk_hbox_new(TRUE, 0);
  gtk_fixed_put(GTK_FIXED(app_ui_data->control), tmp1,
  	CTRL_STATUS_WHITE_LEFT, CTRL_STATUS_WHITE_TOP);

  tmp2 = new Widget();
  tmp2->Attach(tmp1, true);

  app_ui_data->white_status = new Chess_Status();
  app_ui_data->white_status->Set_Active(false);
  app_ui_data->white_status->Open(tmp2);

  osso_log(LOG_DEBUG,"Black player: Position");
  tmp1 = gtk_hbox_new(TRUE, 0);
  gtk_fixed_put(GTK_FIXED(app_ui_data->control), tmp1,
  	CTRL_STATUS_BLACK_LEFT, CTRL_STATUS_BLACK_TOP);

  tmp2 = new Widget();
  tmp2->Attach(tmp1, true);

  app_ui_data->black_status = new Chess_Status();
  app_ui_data->black_status->Set_Active(false);
  app_ui_data->black_status->Open(tmp2);

  osso_log(LOG_DEBUG,"White and Black player: Creating the view");
  ui_player_status(app_ui_data);

  /* Moves list */
  osso_log(LOG_DEBUG,"Last moves list: View label");
  GtkLabel *tmp = GTK_LABEL( gtk_label_new("") );
  gtk_fixed_put(GTK_FIXED(app_ui_data->control), GTK_WIDGET(tmp),
  	CTRL_MOVES_LEFT, CTRL_MOVES_TOP);

  osso_log(LOG_DEBUG,"Last moves list: Component");
  app_ui_data->moves_list = new Chess_Moves_List(tmp);

  /* Overlay Button */
  tmp1 = gtk_drawing_area_new();
  app_ui_data->overlay_button = tmp1;
  gtk_widget_set_size_request(GTK_WIDGET(tmp1), 118, 65);
  gtk_fixed_put(GTK_FIXED(app_ui_data->control), tmp1,
  	CTRL_OVR_BUTTON_LEFT, CTRL_OVR_BUTTON_TOP);
  GError *error = NULL;

  app_ui_data->overlay_back = gtk_icon_theme_load_icon(
		  gtk_icon_theme_get_default(), "general_overlay_back",
		  64, GTK_ICON_LOOKUP_NO_SVG, &error);

  /*gtk_widget_add_events(GTK_WIDGET(tmp1),
		GDK_POINTER_MOTION_MASK |
		GDK_BUTTON_MOTION_MASK |
		GDK_BUTTON_PRESS_MASK |
		GDK_BUTTON_RELEASE_MASK);*/

  g_signal_connect(GTK_WIDGET(app_ui_data->overlay_button),
		  "expose-event", G_CALLBACK(on_overlay_expose),
		  app_ui_data);
#if 0	/* Rama - Commented. BugID# 98732 */
  g_signal_connect(GTK_WIDGET(tmp1), "button_press_event",
		  G_CALLBACK(on_back_button_press), app_ui_data);
  g_signal_connect(GTK_WIDGET(tmp1), "button_release_event",
		  G_CALLBACK(on_back_button_release), app_ui_data);
#endif
}

/* Create the main window */
GtkWindow *ui_create_main_view( void )
{
  GtkWindow *result;

  osso_log(LOG_DEBUG,"Creating window and setting title");

  /* Create GtkWindow */
  result = GTK_WINDOW( gtk_window_new(GTK_WINDOW_TOPLEVEL) );

  g_assert(result);

  /* Set title, this should not be visible anywhere. */
  gtk_window_set_title( result, "ChessUI" );
  
  return result;
}


gboolean end_dialog_focus_out(GtkWidget *widget,
                              GdkEventFocus *,
                              AppUIData *app_ui_data)
{
    if (!focus_is_own())
    {
        gtk_widget_destroy(widget);
        app_ui_data->dialog = NULL;
        chess_pause(NULL,NULL);
        return TRUE;
    }

    return  FALSE;
}


/* Show game end dialog */
void ui_show_end_dialog(AppUIData *app_ui_data, gchar *winner)
{
    if (!app_ui_data) return;

	GtkDialog *dialog = NULL;
	GtkWidget *label = NULL;
	gchar message[MAX_MESSAGE];
	GList* actionList = NULL;

	dialog = GTK_DIALOG( gtk_dialog_new_with_buttons(
		_("game_ti_chess_end_game_title"),
		GTK_WINDOW (app_ui_data->window),
		GtkDialogFlags(GTK_DIALOG_MODAL |
		  GTK_DIALOG_DESTROY_WITH_PARENT |
		  GTK_DIALOG_NO_SEPARATOR),
		dgettext ("hildon-libs", "wdgt_bd_done" ),
		GTK_RESPONSE_OK,
		NULL
		) );
    gtk_window_set_default_size(GTK_WINDOW(dialog), 250, -1);
    g_signal_connect(dialog,"focus-out-event",G_CALLBACK(end_dialog_focus_out), app_ui_data);

  	/*hildon_help_dialog_help_enable(
            GTK_DIALOG(dialog),
            CHESS_HELP_ENDGAME,
            get_app_data()->app_osso_data->osso);*/
	
    g_signal_connect(G_OBJECT(dialog), "key_press_event",
    G_CALLBACK(gtk_widget_destroy), dialog);
	app_ui_data->dialog=dialog;
	if (winner)
		g_snprintf(message, MAX_MESSAGE-1,
		_("game_fi_chess_end_game_description%s"), winner);
	else
		g_snprintf(message, MAX_MESSAGE-1,
		_("game_fi_chess_end_game_tie_description"));
	message[MAX_MESSAGE-1]=0;

	label = gtk_label_new( message);
	hildon_helper_set_logical_color ( GTK_WIDGET(label), GTK_RC_FG, GTK_STATE_NORMAL,"SecondaryTextColor" );
	gtk_container_add( GTK_CONTAINER( dialog->vbox ),
		label);

	
	actionList = gtk_container_get_children( GTK_CONTAINER( dialog->action_area ));
	if (GTK_IS_BUTTON(actionList->data))
	{
		// FIX ME : ButtonTextColor is not working hence using SecondaryTextColor for button label text as well.
		//hildon_helper_set_logical_color ( GTK_WIDGET(GTK_LABEL(GTK_BIN(actionList->data)->child)), GTK_RC_FG, GTK_STATE_NORMAL,"ButtonTextColor" );
		//hildon_helper_set_logical_color ( GTK_WIDGET(GTK_LABEL(GTK_BIN(actionList->data)->child)), GTK_RC_FG, GTK_STATE_PRELIGHT,"ButtonTextColor" );
		
		hildon_helper_set_logical_color ( GTK_WIDGET(GTK_LABEL(GTK_BIN(actionList->data)->child)), GTK_RC_FG, GTK_STATE_NORMAL,"SecondaryTextColor" );
		hildon_helper_set_logical_color ( GTK_WIDGET(GTK_LABEL(GTK_BIN(actionList->data)->child)), GTK_RC_FG, GTK_STATE_PRELIGHT,"SecondaryTextColor" );
	}
	
	
	gtk_widget_show_all(GTK_WIDGET(dialog));

    gtk_dialog_run(dialog);
    
    gtk_widget_destroy( GTK_WIDGET( dialog ));
	g_list_free(actionList);
    app_ui_data->dialog = NULL;
}

/* Clean up */
void ui_clean_up( AppUIData *app_ui_data )
{
  /* Paranoia */
  g_assert( app_ui_data );

  /* There may be other cleanup operations */

  return;
}

/* Show Check infoprint for check */
void ui_show_check_infoprint(AppUIData *app_ui_data)
{
  if (!app_ui_data) return;

  hildon_banner_show_information(GTK_WIDGET(app_ui_data->window),NULL,
    _("game_ib_chess_check"));		
}

/* Show Check infoprint for check mate */
void ui_show_check_mate_infoprint(AppUIData *app_ui_data)
{
  if (!app_ui_data) return;
  hildon_banner_show_information(GTK_WIDGET(app_ui_data->window), NULL,
    _("game_ib_chess_check_mate"));
}

void ui_player_status(AppUIData *app_ui_data)
{
  if (app_ui_data->opponent_type_human) {
    if (app_ui_data->player_color_white) {
      app_ui_data->white_status->Load(cs_white);
      app_ui_data->black_status->Load(cs_black);
      
      app_ui_data->white_status->Set_Active(true);
    } else {
      app_ui_data->white_status->Load(cs_black);
      app_ui_data->black_status->Load(cs_white);
      
      app_ui_data->black_status->Set_Active(true);
    }

    app_ui_data->white_status->Set_Label( _(CHESS_PLAYER1_NAME) );
    app_ui_data->black_status->Set_Label( _(CHESS_PLAYER2_NAME) );    
  } else if (app_ui_data->player_color_white) {
    app_ui_data->white_status->Load(cs_white);
    app_ui_data->black_status->Load(cs_black);
    
    app_ui_data->white_status->Set_Label( _(CHESS_PLAYER1_NAME) );
    app_ui_data->black_status->Set_Label( _(CHESS_COMPUTER_NAME) );
    
    app_ui_data->white_status->Set_Active(true);
  } else {
    app_ui_data->white_status->Load(cs_black);
    app_ui_data->black_status->Load(cs_white);
    
    app_ui_data->white_status->Set_Label( _(CHESS_PLAYER1_NAME) );
    app_ui_data->black_status->Set_Label( _(CHESS_COMPUTER_NAME) );
    
    app_ui_data->black_status->Set_Active(true); 
  }
}
