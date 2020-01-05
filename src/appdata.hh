/**
   @file appdata.h

    Header file for defining the data structures the whole application
    needs to function properly.

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

#ifndef APPDATA_H
#define APPDATA_H

/* GTK */
#include <gtk/gtk.h>

/* Gconf */
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>

/* UI */
#include "babychess/game_editor.hh"
#include "babychess/widget.hh"
#include "babychess/text_button.hh"
#include "ui/chess_button.hh"
#include "ui/chess_status.hh"
#include "ui/chess_moves_list.hh"

/* Sounds */
#include "sounds.h"
#include <canberra.h>

/* Help context */
#define CHESS_HELP_PROMOTEPAWN "//chess/a/4"
#define CHESS_HELP_ENDGAME "//chess/a/5"

/*
  Application UI data

  This structure should contain ALL application UI related data, which
  otherwise would be impossible to pass with events to callbacks. It makes
  complicated intercommunication between widgets possible.

  So when you add a widget to ui, put a pointer to it inside this struct.
*/
typedef struct _AppUIData AppUIData;
struct _AppUIData {
  GtkWindow *window;
  GtkWidget *main_hbox;
  GtkWidget *control;
  GtkWidget *control_image;
  GtkWidget *redo_hbox;
  GtkWidget *undo_hbox;
  GtkWidget *pause_button;
  GtkWidget *overlay_button;
  GtkDialog *dialog;
  Game_Editor *game;
  Widget *bbox;
  Widget *redo_box;
  Widget *undo_box;
  Chess_Button *undo_button;
  Chess_Button *redo_button;
  Chess_Status *white_status;
  Chess_Status *black_status;
  Chess_Moves_List *moves_list;
  GdkPixbuf *overlay_back;
  
  gboolean player_color_white;
  gboolean show_legal_moves;
  gboolean opponent_type_human;
  gboolean enable_sound;
  gint difficulty_level;
  gchar *save_file;  
  
  gboolean game_loading;
  gboolean decrease_undo;
  gboolean mate_pos;
};

typedef struct _AppOSSOData AppOSSOData;
struct _AppOSSOData {
  osso_context_t *osso;
};

typedef struct _AppGConfData AppGConfData;
struct _AppGConfData {
  GConfClient *gc_client;
};

typedef struct _AppSoundData AppSoundData;
struct _AppSoundData {
  int sound_ids[SOUND_COUNT];
  ca_context *ca;
  ca_proplist *pl;
};

/*
  Application data structure. Pointer to this is passed eg. with UI
  event callbacks.
*/
typedef struct _AppData AppData;
struct _AppData {
  AppUIData *app_ui_data;
  AppOSSOData *app_osso_data;
  AppGConfData *app_gconf_data;
  AppSoundData *app_sound_data;
};

#endif /* APPDATA_H */
