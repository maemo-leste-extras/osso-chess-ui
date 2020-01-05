/**
  @file plugin.c
  
  The plugin for the games-startup

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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/vfs.h>
#include <libgen.h>
#include <gtk/gtk.h>

#include <gconf/gconf.h>
#include <gconf/gconf-client.h>

#include <libgnomevfs/gnome-vfs.h>

#include <glib.h>
#include <glib/gstdio.h>
#include <glib-object.h>
#include <gdk/gdkkeysyms.h>
#include <hildon/hildon-helper.h>

/* Hildon */
#include <hildon/hildon.h>
#include <hildon/hildon-file-chooser-dialog.h>
#include <startup_plugin.h>
#include <comapp_common.h>
#include <comapp_opensave.h>
#define D_(s) dgettext ("hildon-fm", s)
#define COMBOBOX_HEIGHT 40
#define COMBOBOX_WIDTH 200
#define LABEL_HEIGHT 50
#define LABEL_WIDTH -1

#define SETTINGS_ENABLE_SOUND              "/apps/osso/chess/enable_sound"
#define SETTINGS_SHOW_LEGAL_MOVES          "/apps/osso/chess/show_legal_moves"
#define SETTINGS_PLAYER_COLOR              "/apps/osso/chess/player_color_white"
#define SETTINGS_OPPONENT_HUMAN            "/apps/osso/chess/opponent_type_human"
#define SETTINGS_LEVEL                     "/apps/osso/chess/difficulty_level"

#define SETTINGS_CHESS_SAVE_FOLDER_DEFAULT ".documents"
#define OSSO_CHESS_HELP_PATH "//chess/a/1"
#define OSSO_CHESS_HELP_LOADFILE           "//chess/a/3"
#define OSSO_CHESS_HELP_SAVEFILE           "//chess/a/2"
#define CHESS_MIMETYPE                     "application/game"
#define CHESS_FILE_GLOB                    "*.game"
#define SETTINGS_CHESS_SAVE_FILE           "/apps/osso/chess/save_file"
#define DEFAULT_STATE_SAVE_FILE            "/tmp/osso-chess-save"
#define CHESS_PAUSE_SAVE_FILE_TMP "/tmp/chess_state_file.pgn"
#define CHESS_SAVE_FILE_TMP "/tmp/osso_chess.tmp"
#define CHESS_SERVICE "com.nokia.osso_chess"
#define CHESS_OBJPATH "/com/nokia/osso_chess"
#define CHESS_IFACE "com.nokia.osso_chess"

/* When a (saved or new) game is loading, applicatin won't let the user start
 * a new one with full-screen hw-key. This gconf key is set to TRUE when
 * loading starts and set back to FALSE when finished. */
#define SETTINGS_CHESS_LOAD_IN_PROGRESS "/apps/osso/chess/load_in_progress"

#define GAME_SAVE_METHOD     "game_save"
#define GAME_LOAD_METHOD     "game_load"

#define URI_FILE_PREFIX   "file://"
#define MMC_MOUNTPOINT_ENV "MMC_MOUNTPOINT"
gboolean is_mmc;
gchar *mmc_uri;

GtkWidget *fc_dialog = NULL;

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef ENABLE_NLS
#include <libintl.h>
#ifdef gettext_noop
#define N_(String) gettext_noop(String)
#else
#define N_(String) (String)
#endif
#else /* NLS is disabled */
#define _(String) (String)
#define N_(String) (String)
#define textdomain(String) (String)
#define gettext(String) (String)
#define dgettext(Domain,String) (String)
#define dcgettext(Domain,String,Type) (String)
#define bindtextdomain(Domain,Directory) (Domain) 
#define bind_textdomain_codeset(Domain,Codeset) (Codeset) 
#endif /* ENABLE_NLS */

#define _(String)                 dgettext("osso-games", String)
#define __(String)                dgettext("hildon-common-strings", String)

#define CHESS_MENU_ITEMS          0

#define CHESS_MENU_HUMAN_WHITE         0
#define CHESS_MENU_HUMAN_BLACK         1
#define CHESS_MENU_OPPONENT_EASY       2
#define CHESS_MENU_OPPONENT_MEDIUM     3
#define CHESS_MENU_OPPONENT_HARD       4
#define CHESS_MENU_OPPONENT_EXPERT     5
#define CHESS_MENU_OPPONENT_HUMAN      6
#define CHESS_MENU_SOUND               7
#define CHESS_MENU_SHOW_LEGAL_MOVES    8

/* menu_action identifiers */
#define MA_GAME_PLAY      1
#define MA_GAME_SAVEMENU_REFERENCE 17
#define ME_GAME_OPEN     20
#define ME_GAME_SAVE     21
#define ME_GAME_SAVE_AS  22
#define MA_GAME_BG_KILL    39

#define MA_GAME_PLAYING_START 30
#define MA_GAME_PLAYING 31
#define MA_WAIT_FOR_SAVE 32

#define MA_LOAD_MIME 34
#define MA_GAME_RESET_OLD_SETTINGS 35
#define GAME_PAUSED      2
#define MA_GAME_ENDED 37
#define MA_GAME_RESTORE_LAST_SETTINGS 38


enum { chess_human_invalid=-1, chess_human_white=0, chess_human_black };
enum { chess_opponent_1=0, chess_opponent_2, chess_opponent_3,
       chess_opponent_4, chess_opponent_human };

enum {
  GAME_SAVE_NONE=1,
  GAME_SAVE_SAVING,
  GAME_SAVE_OK,
  GAME_SAVE_ERR,
  GAME_SAVE_CANCEL
};

enum {
  GAME_LOAD_FILE_UNSUPPORTED=1,
  GAME_LOAD_FILE_NOT_FOUND
};

GConfClient *gcc = NULL;
GtkWidget *human_box = NULL; 
GtkWidget *opponent_box = NULL;
GtkWidget *sound_check = NULL;
GtkWidget *show_legal_moves_check = NULL;
gchar *chess_title  = NULL;
osso_context_t *osso;

//Menu
GtkWidget *menu_items[CHESS_MENU_ITEMS];
GtkWidget *saving_progress_bar;
GtkWidget *human_label;
GtkWidget *opponent_label;
GtkWidget *sound_label;
GtkWidget *show_legal_moves_label;
GtkWidget *save_item = NULL;
	
GSList * group = NULL;
GSList * opponent_group = NULL;

gboolean is_new_file;

GnomeVFSHandle *read_handle=NULL;
GnomeVFSMonitorHandle *monitor_handle=NULL;
   
gint selected_difficulty = -1;
gint selected_player = chess_human_invalid;

static GtkWidget  *load_plugin                      (void);
static void       unload_plugin                     (void);
static void       write_config                      (void);
static void       update_menu                       (void);
static void       game_load_cb                      (void);
static void       game_save_cb                      (void);
static void       game_save_as_cb                   (void);
static void       game_save                         (void);
static void       plugin_ui_hide_saving_dialog             (void);
static void       ui_show_nomem_dialog              (void);
static void       plugin_ui_show_saving_dialog             (void);
static gboolean   ui_set_saving_dialog_progress     (gfloat       progress);
static GtkWidget  **load_menu                       (guint       *nitems);
static void       game_load                         (gchar       *file);
static gboolean   ui_show_replace_file_dialog       (gchar       *file);
static gboolean   ui_show_file_chooser              (gboolean     open);
static void       chess_human_cb                    (GtkWidget   *widget, 
                                                     gpointer     data);
static void       plugin_cb                         (GtkWidget   *menu_item,
                                                     gpointer     cb_data);
static GtkWidget *get_ui_cb                         (void);

static gchar*     settings_get_string               (const gchar *key);
static void       plugin_volume_unmounted_cb        (GnomeVFSVolumeMonitor *vfsvolumemonitor,
                                                     GnomeVFSVolume *volume, gpointer user_data);
static void       plugin_ui_show_save_nommc_dialog	  (void);
static void       plugin_ui_show_load_err             (int err_type, gchar *loadfile);
static gboolean   plugin_settings_get_bool_fallback	  (gchar *key, gboolean fall);
static void       plugin_get_settings                 (void);
static GConfValue *plugin_settings_get				  (const gchar *key);
static gboolean   plugin_settings_get_bool			  (const gchar *key);
static gint 	  plugin_settings_get_int_fallback	  (gchar *key, gint fall);
static gchar* 	  plugin_settings_get_string		  (const gchar *key);
static gint 	  plugin_settings_get_int			  (const gchar *key);
static void 	  plugin_show_infoprint				  (gchar *msg);
static void 	  plugin_set_settings				  (void);
static void 			set_all_insensitive							(void);
static void 			plugin_ui_cancel_saving(void);
static void				plugin_saving_dialog_cb(GtkDialog *dialog, gint button, gpointer user_data);
static gboolean 	plugin_settings_set_bool(const gchar *key, const gboolean val);
static gboolean 	plugin_settings_set_string(const gchar *key, const gchar *val);
static gboolean 	plugin_settings_set_int(const gchar *key, const gint val);
static void 			plugin_mime_open(void);
static void plugin_restore_original_settins(void);
static void plugin_restore_last_settings(void);
static void 			plugin_wait_for_save(void);
gboolean    plugin_select_separator  (GtkTreeModel *model,GtkTreeIter *iter,gpointer data);
gboolean    plugin_sound_keypress_cb      (GtkWidget   *widget,GdkEventKey *event,gpointer     user_data);
gboolean    plugin_opponent_keypress_cb      (GtkWidget   *widget,GdkEventKey *event,gpointer     user_data);
static void file_monitor_cb( GnomeVFSMonitorHandle * handle,
                            	const gchar * monitor_uri, const gchar * info_uri,
                            	GnomeVFSMonitorEventType event_type, gpointer user_data );

static StartupPluginInfo plugin_info = {
  load_plugin,
  unload_plugin,
  write_config,
  load_menu,
  update_menu,
  plugin_cb,
  get_ui_cb
};

GameStartupInfo gs;

STARTUP_INIT_PLUGIN(plugin_info, gs, FALSE, TRUE)

static int changed = FALSE;
static gchar *chess_save_folder;
static gboolean chess_saved;
static gint chess_save_done;
static GtkWidget *chess_saving_dialog;



/* Set int type value */
static gboolean plugin_settings_set_int(const gchar *key, const gint val)
{
  return gconf_client_set_int(gcc, key, val, NULL);
}


/* Set string type value */
static gboolean plugin_settings_set_string(const gchar *key, const gchar *val)
{
  return gconf_client_set_string(gcc, key, val, NULL);
}


static gboolean plugin_settings_set_bool(const gchar *key, const gboolean val)
{
  return gconf_client_set_bool(gcc, key, val, NULL);
}


/* Get int type key value */
static gint plugin_settings_get_int(const gchar *key)
{
  return gconf_client_get_int(gcc, key, NULL);
}

/* Get string type key value */
static gchar* plugin_settings_get_string(const gchar *key)
{
  return gconf_client_get_string(gcc, key, NULL);
}

/**
 Helper function to get an int entry, returning defined value, if not found.
 @param key GConf key to be get.
 @param fall Fallback to this, if not found, or invalid.
 @return Value got from GConf or the value specified in fall.
*/
static gint plugin_settings_get_int_fallback(gchar *key, gint fall)
{
  GConfValue *gc_val = plugin_settings_get(key);
  if (gc_val)
  {
    if (gc_val->type == GCONF_VALUE_INT) {
      gconf_value_free(gc_val);
      return plugin_settings_get_int(key);
    }
    gconf_value_free(gc_val);
  }
  return fall;
}

/* Get boolean type key value */
static gboolean plugin_settings_get_bool(const gchar *key)
{
  return gconf_client_get_bool(gcc, key, NULL);
}

/* Get key value */
static GConfValue *plugin_settings_get(const gchar *key)
{
  return gconf_client_get(gcc, key, NULL);
}

/**
 Helper function to get an bool entry, returning defined value, if not found.
 @param key GConf key to be get.
 @param fall Fallback to this, if not found, or invalid.
 @return Value got from GConf or the value specified in fall.
*/
static gboolean plugin_settings_get_bool_fallback(gchar *key, gboolean fall)
{
  GConfValue *gc_val = plugin_settings_get(key);
  
  if (gc_val)
  {
    if (gc_val->type == GCONF_VALUE_BOOL) {
      gconf_value_free(gc_val);
      return plugin_settings_get_bool(key);
    }
    gconf_value_free(gc_val);
  }
  return fall;
}

/**
  Get settings from GConf.

 */
static void plugin_get_settings(void)
{
  gboolean white = TRUE;
  gboolean human = FALSE;
  gboolean sound = FALSE;
  gboolean moves = FALSE;
  gint player = -1;
  gint difficulty = -1;

    /* Get settings */
    white = plugin_settings_get_bool_fallback(SETTINGS_PLAYER_COLOR, TRUE);
    sound = plugin_settings_get_bool_fallback(SETTINGS_ENABLE_SOUND, TRUE);

    difficulty = plugin_settings_get_int_fallback(SETTINGS_LEVEL, 4);

    human = plugin_settings_get_bool(SETTINGS_OPPONENT_HUMAN);
    moves = plugin_settings_get_bool(SETTINGS_SHOW_LEGAL_MOVES);

    /* Sound */
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON( sound_check), sound );

    /* Legal moves */
    gtk_toggle_button_set_active(
      GTK_TOGGLE_BUTTON( show_legal_moves_check), moves );

    /* Human color */
    if (white == TRUE) {
      player = chess_human_white;
    } else {
      player = chess_human_black;
    }
    gtk_combo_box_set_active(
      GTK_COMBO_BOX( human_box ), player );
    
    if (difficulty==4)
    {
      difficulty=5;
    }

    /* Opponent difficulty and type */
    gtk_combo_box_set_active(
      GTK_COMBO_BOX( opponent_box ), difficulty );
}


gboolean    plugin_opponent_keypress_cb      (GtkWidget   *widget,
                                            GdkEventKey *event,
                                            gpointer     user_data)
{
  widget=NULL;
  user_data=NULL;
  if (event->keyval==GDK_Down)
  {
    gtk_widget_grab_focus(sound_label);
    return TRUE;
  }
  return FALSE;
}

gboolean    plugin_sound_keypress_cb      (GtkWidget   *widget,
                                            GdkEventKey *event,
                                            gpointer     user_data)
{
  widget=NULL;
  user_data=NULL;
  if (event->keyval==GDK_Up)
  {
    gtk_widget_grab_focus(opponent_label);
    return TRUE;
  }
  return FALSE;
}


gboolean    plugin_select_separator  (GtkTreeModel *model,
                                             GtkTreeIter *iter,
                                             gpointer data)
{
   GtkTreePath *path;
   gboolean result;
    data=NULL;
   path = gtk_tree_model_get_path (model, iter);
   result = gtk_tree_path_get_indices (path)[0] == 4;
   gtk_tree_path_free (path);
   return result;
}


/* Load the plugin, create the UI */
static GtkWidget *
load_plugin (void)
{
  GtkWidget *game_hbox, *game_vbox;
  GtkSizeGroup *groupleft= GTK_SIZE_GROUP( gtk_size_group_new( GTK_SIZE_GROUP_BOTH ) ),*groupright= GTK_SIZE_GROUP( gtk_size_group_new( GTK_SIZE_GROUP_BOTH ) );
  int enable_sound, show_legal_moves, color_white,  difficulty_level;
  gboolean gnomevfs_ok;  

	if (!gnome_vfs_initialized()) {
    gnomevfs_ok = gnome_vfs_init();
    if (!gnomevfs_ok) {
        puts("GnomeVFS init failed");
        return NULL;
    }
  }

  bindtextdomain (GETTEXT_PACKAGE, CHESSLOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  gcc = gconf_client_get_default();
	enable_sound = plugin_settings_get_bool_fallback(SETTINGS_ENABLE_SOUND,TRUE);
	show_legal_moves =plugin_settings_get_bool_fallback(SETTINGS_SHOW_LEGAL_MOVES,FALSE);
	color_white = !plugin_settings_get_bool_fallback(SETTINGS_PLAYER_COLOR,TRUE);
  difficulty_level = plugin_settings_get_int_fallback(SETTINGS_LEVEL,0);

  game_vbox = gtk_vbox_new (FALSE, 0);
  g_assert (game_vbox);

  game_hbox = gtk_hbox_new (TRUE, 0);
  g_assert (game_hbox);

  
  

  human_box = gtk_combo_box_new_text ();
  g_assert (human_box);

  gtk_combo_box_append_text (GTK_COMBO_BOX (human_box), 
    _("game_va_chess_settings_human_white")); //  "White");
  gtk_combo_box_append_text (GTK_COMBO_BOX (human_box), 
    _("game_va_chess_settings_human_black")); //  "Black");
  gtk_widget_set_size_request(human_box,
    COMBOBOX_WIDTH, COMBOBOX_HEIGHT);
  gtk_combo_box_set_active (GTK_COMBO_BOX (human_box), color_white);
  human_label =hildon_caption_new(groupleft,_("game_fi_chess_settings_human"),human_box,NULL,HILDON_CAPTION_OPTIONAL);
	g_assert(human_label);

  gtk_box_pack_start (GTK_BOX (game_hbox), game_vbox, FALSE, FALSE, 2);
  gtk_box_pack_start (GTK_BOX (game_vbox), human_label, FALSE, FALSE, 2);

  sound_check = gtk_check_button_new ();
  g_assert (sound_check);
  sound_label = hildon_caption_new(groupright,_("game_fi_chess_settings_sound"),sound_check,NULL,HILDON_CAPTION_OPTIONAL);
	g_assert (sound_label);

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(sound_check), enable_sound);

  opponent_box = gtk_combo_box_new_text ();
  g_assert (opponent_box);
  gtk_combo_box_set_row_separator_func(GTK_COMBO_BOX(opponent_box),plugin_select_separator,NULL,NULL);

  gtk_combo_box_append_text (GTK_COMBO_BOX (opponent_box),
    _("game_va_chess_settings_opponent_easy")); //  "Easy");
  gtk_combo_box_append_text (GTK_COMBO_BOX (opponent_box),
    _("game_va_chess_settings_opponent_medium")); //  "Medium");
  gtk_combo_box_append_text (GTK_COMBO_BOX (opponent_box),
    _("game_va_chess_settings_opponent_hard")); //  "hard");
  gtk_combo_box_append_text (GTK_COMBO_BOX (opponent_box),
    _("game_va_chess_settings_opponent_expert")); //  "Expert");
  gtk_combo_box_append_text (GTK_COMBO_BOX (opponent_box),
    "---"); //  "Expert");
  gtk_combo_box_append_text (GTK_COMBO_BOX (opponent_box),
    _("game_va_chess_settings_opponent_human")); //  "Human player");
  gtk_widget_set_size_request(opponent_box,
    COMBOBOX_WIDTH, COMBOBOX_HEIGHT);
  if (difficulty_level==4)
  {
    difficulty_level=5;
  }
    gtk_combo_box_set_active (GTK_COMBO_BOX (opponent_box), difficulty_level);
	opponent_label =hildon_caption_new(groupleft,_("game_fi_chess_settings_opponent"),opponent_box,NULL,HILDON_CAPTION_OPTIONAL);
	g_assert(opponent_label);

	gtk_box_pack_start (GTK_BOX (game_vbox),   opponent_label, FALSE, FALSE, 2);

  game_vbox = gtk_vbox_new (FALSE, 0);
	g_assert(game_vbox);
	
	gtk_box_pack_start (GTK_BOX (game_hbox), game_vbox, FALSE, FALSE, 2);
	gtk_box_pack_start (GTK_BOX (game_vbox),  sound_label, FALSE, FALSE, 2);
  
  show_legal_moves_check = gtk_check_button_new ();
  g_assert (show_legal_moves_check);

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(show_legal_moves_check), show_legal_moves);
	show_legal_moves_label = hildon_caption_new(groupright,_("game_fi_chess_settings_legal_moves"),show_legal_moves_check,NULL,HILDON_CAPTION_OPTIONAL);
	g_assert (show_legal_moves_label);
  gtk_widget_set_size_request(show_legal_moves_label,
    LABEL_WIDTH, LABEL_HEIGHT);
		
  gtk_box_pack_start (GTK_BOX (game_vbox), show_legal_moves_label, FALSE, FALSE, 2);

	gtk_widget_show_all (game_hbox);
  g_signal_connect (G_OBJECT(human_box), "changed",
      G_CALLBACK(chess_human_cb), NULL);

  g_signal_connect (G_OBJECT(opponent_box), "key-press-event",
      G_CALLBACK(plugin_opponent_keypress_cb), NULL);
  g_signal_connect (G_OBJECT(sound_check), "key-press-event",
      G_CALLBACK(plugin_sound_keypress_cb), NULL);
  plugin_settings_set_string(SETTINGS_CHESS_SAVE_FILE, "");
  plugin_settings_set_bool(SETTINGS_CHESS_LOAD_IN_PROGRESS, FALSE);

	return game_hbox;  
}

static void
unload_plugin (void)
{

	plugin_restore_last_settings();

	if( monitor_handle ) {
		gnome_vfs_monitor_cancel( monitor_handle );
		monitor_handle = NULL;
	}

	if (read_handle)
	{
		gnome_vfs_close(read_handle);
    read_handle=NULL;
	}
	write_config();
}

/* The following function is from INDT's games-startup code. The copyrights 
belongs to them. The INDT's games-startup was released under LGPL license. 
This function is modified under LGPL license*/

/* Write the state of the UI */
static void 
write_config (void)
{
  int combo;

  combo = gtk_combo_box_get_active (GTK_COMBO_BOX (opponent_box));	  
    gconf_client_set_bool (gcc, SETTINGS_ENABLE_SOUND, 
	gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (sound_check)), NULL);
    gconf_client_set_bool (gcc, SETTINGS_SHOW_LEGAL_MOVES, 
	gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (show_legal_moves_check)), NULL);
  if (combo==5)
  {
    combo=4;
  }
    gconf_client_set_int (gcc, SETTINGS_LEVEL, combo, NULL);
    gconf_client_set_bool (gcc, SETTINGS_PLAYER_COLOR, 
    	!gtk_combo_box_get_active (GTK_COMBO_BOX (human_box)), NULL);
	if (combo == 4) {
		gconf_client_set_bool (gcc, SETTINGS_OPPONENT_HUMAN, 1, NULL);
	}
	else
	{
		gconf_client_set_bool (gcc, SETTINGS_OPPONENT_HUMAN, 0, NULL);
	}
}


/* Creates the menu to add that to the main application */
static GtkWidget **
load_menu (guint *nitems)
{
  *nitems = CHESS_MENU_ITEMS;
  return menu_items;
}

/* The following function is from INDT's games-startup code. The copyrights 
belongs to them. The INDT's games-startup was released under LGPL license.*/

/** Update the menu if something is changed in the UI */
static void
update_menu (void)
{
}

/* The following function is from INDT's games-startup code. The copyrights 
belongs to them. The INDT's games-startup was released under LGPL license. 
This function is modified under LGPL license*/

/** When the human setting is changed, then change the menu too */
static void 
chess_human_cb (GtkWidget *widget, 
                gpointer   data)
{
  if (!changed) {
      changed = TRUE;
      gint active = gtk_combo_box_get_active (GTK_COMBO_BOX (widget));
      if (active != CHESS_MENU_HUMAN_BLACK)
        gtk_combo_box_set_active (GTK_COMBO_BOX (human_box), (int) data);
  }
  changed = FALSE;
}

/* Callback function for the load menu */
static void game_load_cb(void)
{
  if (ui_show_file_chooser(TRUE)) {
    plugin_set_settings();
  }
}

/* Callback to the save menu */
static void game_save_cb(void)
{
  gchar *file = settings_get_string(SETTINGS_CHESS_SAVE_FILE);
  GnomeVFSFileInfo *file_info = gnome_vfs_file_info_new();
  GnomeVFSResult vfs_res = file ? gnome_vfs_get_file_info(file,
      file_info,
      GNOME_VFS_FILE_INFO_FOLLOW_LINKS) : GNOME_VFS_ERROR_GENERIC;
	if (read_handle)
	{
		gnome_vfs_close(read_handle);
		read_handle=NULL;
	}
  plugin_set_settings();
  if (vfs_res == GNOME_VFS_OK &&
      file_info->type == GNOME_VFS_FILE_TYPE_REGULAR) {
    is_new_file = FALSE;
    game_save();
  }
  else if (ui_show_file_chooser(FALSE) == TRUE) {
    is_new_file = TRUE;
    game_save();
  }

  gnome_vfs_file_info_unref(file_info);

  if (file != NULL) 
    g_free(file);
}

/* Callback to the save_as menu */
static void game_save_as_cb(void)
{
  gchar *old_filename = settings_get_string(SETTINGS_CHESS_SAVE_FILE);
  if (read_handle) {
	gnome_vfs_close(read_handle);
	read_handle=NULL;
  }
  
  if (ui_show_file_chooser(FALSE) == TRUE) {
    gchar *new_filename = plugin_settings_get_string(SETTINGS_CHESS_SAVE_FILE);
    if (g_strcasecmp(old_filename, new_filename) != 0) {
      is_new_file = TRUE;
    } else {
      is_new_file = FALSE;
    }
    
	if( monitor_handle ) {
		gnome_vfs_monitor_cancel( monitor_handle );
		monitor_handle = NULL;
	}    
    gnome_vfs_monitor_add( &monitor_handle, new_filename,
    	GNOME_VFS_MONITOR_FILE, file_monitor_cb, NULL );
                                                         
    if (new_filename) {
      g_free(new_filename);
    }
    game_save();
        
  }
  
  if (old_filename) {
    g_free(old_filename);
  }
}

/**
   Check that game file has has correct magic. This doesn't use engine, but it
   might be good idea to do proper validation there.
   @param filename URI to file
   @return TRUE if file is/was valid
 */
static gboolean
validate_chess_file( const gchar * filename ) {

	GnomeVFSHandle * handle = NULL;
	gchar buffer[ 64 ];
    GnomeVFSFileSize size;
    guint at;
    GnomeVFSResult ret;

	ret = gnome_vfs_open( &handle, filename, GNOME_VFS_OPEN_READ );
	if( ret != GNOME_VFS_OK ) {

	
		return FALSE;
	}
	
	/* Read first 64 bytes (should be enough always) */
	gnome_vfs_read( handle, buffer, 64, &size );
	gnome_vfs_close(handle);
	
	/* Skip the empty chars (shouldn't be any but to be sure) */
	for( at = 0; at < size; ++at ) {
		gchar c = buffer[at];
		if( c != ' ' && c != '\t' && c != '\n' && c != '\r' ) {
			break;
		}
	}
	
	/* Should be at least 9 bytes left in file */
	if( (gint)size - (gint)at < 9 ) {
		return FALSE;
	}
	
	/* Just check the magic now */
	if( strncmp( buffer + at, "[Event ", 7 ) == 0 ) {
	
		return TRUE;
		
	} else {
	
       	gchar * name = 
       		comapp_common_get_display_name( filename, NULL, TRUE );
       	gchar * msg = g_strdup_printf( _("game_ni_unsupported_file%s" ),
       		name );
       	g_free( name );
       	plugin_show_infoprint( msg );
       	g_free( msg );
       	
		g_warning( "File '%s' not identified as chess game file.", filename );
		return FALSE;       	
       	
   }
	
}

/* The filechooser dialog */
static gboolean 
ui_show_file_chooser(gboolean open)
{
    
    
    ComappOpenSave opensave_data;
    gchar *mime_types[]={CHESS_MIMETYPE,NULL};
    gchar *buf;
    StartupApp *app=NULL;
	app=gs.ui->app;
    memset(&opensave_data,0,sizeof(ComappOpenSave));
    opensave_data.open_window_title=_("game_ti_load_game_title");
    opensave_data.save_window_title=_("game_ti_save_game_title");
    opensave_data.action=open?GTK_FILE_CHOOSER_ACTION_OPEN:GTK_FILE_CHOOSER_ACTION_SAVE;
    opensave_data.empty_text=_("sfil_va_select_object_no_objects_games");
    opensave_data.osso=app->osso;
    opensave_data.open_help_title=OSSO_CHESS_HELP_LOADFILE;
    opensave_data.save_help_title=OSSO_CHESS_HELP_SAVEFILE;
    opensave_data.mime_types=mime_types;
    opensave_data.parent=GTK_WIDGET(gs.ui->hildon_appview);
    
    
    /* Get & set default folder */
    if (!chess_save_folder || !*chess_save_folder)
    {
	gchar *folder_path = NULL;
      if (chess_save_folder)
        g_free(chess_save_folder);
      folder_path = g_build_filename((const gchar*)getenv("MYDOCSDIR"),
          SETTINGS_CHESS_SAVE_FOLDER_DEFAULT, NULL);
      chess_save_folder = gnome_vfs_escape_path_string(folder_path);
      g_free(folder_path);
    }

    buf=plugin_settings_get_string(SETTINGS_CHESS_SAVE_FILE);
    if (!buf||!strlen(buf))
    {
      if (buf)
        g_free(buf);
      buf = g_strdup_printf("%s.game", _("sfil_va_save_object_name_stub_chess"));
    }
    else
    {
      gchar *fname=NULL;
      fname=g_strdup(g_strrstr(buf,"/")+1);
      g_free(buf);
      buf=fname;
    }
    
    opensave_data.current_uri=g_strconcat(chess_save_folder,"/",buf,NULL);
    
    comapp_opensave_new(&opensave_data);

    /* Rama - changed here */
    fc_dialog = opensave_data.fc_dialog;
    if (comapp_opensave_run(&opensave_data)==COMAPP_FILE_CHOOSER_SELECTED)
    {
        
        gchar *filename = NULL;
        gchar *pathname = NULL;
	filename = opensave_data.result_uri;

	/* Rama - changed here */
	fc_dialog = NULL;
        if (filename == NULL)
        {
            return FALSE;
        }
    
        if (!open)
        {
	        GnomeVFSURI *test_uri = gnome_vfs_uri_new(filename);
      
            if (gnome_vfs_uri_exists(test_uri)) {
                if (!ui_show_replace_file_dialog(filename))
                    {
                        gnome_vfs_uri_unref(test_uri);
                        g_free(filename);

                        return FALSE;
                    }
                }
            gnome_vfs_uri_unref(test_uri);
        }

        pathname = g_strdup(filename);
        strrchr(pathname,'/')[0]='\0';

        if (pathname != NULL)
        {
            g_free(chess_save_folder);
            chess_save_folder = pathname;
        }
        
		if( monitor_handle ) {
			gnome_vfs_monitor_cancel( monitor_handle );
			monitor_handle = NULL;
		}    
	    gnome_vfs_monitor_add( &monitor_handle, filename,
	    	GNOME_VFS_MONITOR_FILE, file_monitor_cb, NULL );
        
        /* Set filename */
        if (!plugin_settings_set_string(SETTINGS_CHESS_SAVE_FILE, filename) ) {
            /* Free */
            g_free(filename);
            return FALSE;
        }
      
        if (open)
        {
            game_load(filename);
            g_free(filename);

            return TRUE;
        }
    }
    else
    {
	/* Rama - changed here */
    	fc_dialog = NULL;
        return FALSE;
    }
    
    return TRUE;
}



static gboolean
key_press_for_cancelling_dialog(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    if(event == NULL)
        return FALSE;
    if (event->keyval == GDK_Escape) {
        gtk_dialog_response(GTK_DIALOG(widget), GTK_RESPONSE_CANCEL);
    	/* Rama - changed here */
	fc_dialog = NULL;
        return TRUE;
		
    }

    data = 0;
    return FALSE;
}

/* Show the replace dialog */
static gboolean 
ui_show_replace_file_dialog(gchar *file)
{
  gint answer;
  gchar *fname=NULL,*bname=NULL;
  bname=g_path_get_basename(file);
  fname=gnome_vfs_unescape_string(bname,NULL);
  g_free(bname);
  gchar *tmp = g_strdup_printf( D_("docm_nc_replace_file"));
  g_free(fname);

  /*replacing the unused logical string*/
  HildonNote *note = HILDON_NOTE(hildon_note_new_confirmation(
        GTK_WINDOW(gs.ui->hildon_appview),
	tmp));

 /* Rama - changed here */
 fc_dialog = note;
		  
  g_signal_connect(G_OBJECT(note), "key-press-event", G_CALLBACK(key_press_for_cancelling_dialog), NULL);
  answer = gtk_dialog_run(GTK_DIALOG(note));
  gtk_widget_destroy(GTK_WIDGET(note));

  /* Rama - changed here */
  fc_dialog = NULL;
  g_free(tmp);
  return (GTK_RESPONSE_OK == answer);
}

static void set_all_insensitive(void)
{
  gtk_widget_set_sensitive(human_label,FALSE);
	gtk_widget_set_sensitive(opponent_label,FALSE);
	gtk_widget_set_sensitive(sound_label,FALSE);
	gtk_widget_set_sensitive(show_legal_moves_label,FALSE);
	if (GTK_WIDGET_VISIBLE(gs.ui->play_button))
	{
		gtk_widget_set_sensitive(gs.ui->play_button,FALSE);
	} 
	if (GTK_WIDGET_VISIBLE(gs.ui->restart_button))
	{
		gtk_widget_set_sensitive(gs.ui->restart_button,FALSE);
	}

    
}

static void set_all_sensitive(void)
{
  gtk_widget_set_sensitive(human_label,TRUE);
	gtk_widget_set_sensitive(opponent_label,TRUE);
	gtk_widget_set_sensitive(sound_label,TRUE);
	gtk_widget_set_sensitive(show_legal_moves_label,TRUE);
    if (GTK_WIDGET_VISIBLE(gs.ui->play_button))
	{
		gtk_widget_set_sensitive(gs.ui->play_button,TRUE);
	}
}

/* The following function is from INDT's games-startup code. The copyrights 
belongs to them. The INDT's games-startup was released under LGPL license. */

static void plugin_wait_for_save(void)
{
	if ((chess_save_done==GAME_SAVE_SAVING)) {
    /* Show startup screen */
    gtk_window_present(GTK_WINDOW(gs.ui->hildon_appview));
    while(gtk_events_pending())
      gtk_main_iteration();
  }
	return;
}

static void plugin_mime_open(void)
{
	gchar *filename=NULL;
	filename=settings_get_string(SETTINGS_CHESS_SAVE_FILE);
	game_load(filename);
	g_free(filename);
	return;
}


/**
  Restores the last selected settings, which was selected by the user before
  continue the game
*/
static void
plugin_restore_last_settings(void)
{
    if ((selected_difficulty != -1))
    {
        gtk_combo_box_set_active(GTK_COMBO_BOX(opponent_box),
                                 selected_difficulty);

        selected_difficulty = -1;
    }

    if ((selected_player != chess_human_invalid))
    {
        gtk_combo_box_set_active(GTK_COMBO_BOX(human_box),
                                 selected_player);

        selected_player = chess_human_invalid;
    }
}

/**
  Restores the original settings, which is stored in the gconf, and it prints
  an infoprint
*/
static void plugin_restore_original_settins(void)
{
    gint difficulty_original_settings =
        plugin_settings_get_int_fallback(SETTINGS_LEVEL, 4);
    gint difficulty_current_value =
        gtk_combo_box_get_active(GTK_COMBO_BOX(opponent_box));
    if (4 == difficulty_original_settings) difficulty_original_settings = 5;
    if (difficulty_original_settings != difficulty_current_value)
    {
        selected_difficulty = difficulty_current_value;

        plugin_show_infoprint(_("game_ib_changes_effect_next_game"));
        gtk_combo_box_set_active(GTK_COMBO_BOX(opponent_box),
                                 difficulty_original_settings);
    }

    gboolean player_original_settings =
        plugin_settings_get_bool(SETTINGS_PLAYER_COLOR);
    gboolean player_current_value =
        ((gtk_combo_box_get_active(GTK_COMBO_BOX(human_box)) == 
            chess_human_white) ? TRUE : FALSE);
    if (player_original_settings != player_current_value)
    {
        selected_player =
            player_current_value ? chess_human_white : chess_human_black;

        plugin_show_infoprint(_("game_ib_changes_effect_next_game"));
        gtk_combo_box_set_active(GTK_COMBO_BOX(human_box),
                                 player_original_settings ?
                                    chess_human_white :
                                    chess_human_black);
    }

}

static GtkWidget *get_ui_cb(void)
{
	return((GtkWidget *)fc_dialog);
	
}


/* Called when a menu item activated */
static void 
plugin_cb(GtkWidget *menu_item, gpointer cb_data)
{
  switch( (int) cb_data ) {
    case ME_GAME_OPEN:
      game_load_cb();
      break;
    case ME_GAME_SAVE:
      game_save_cb();
      break;
    case ME_GAME_SAVE_AS:
      game_save_as_cb();
      break;
	case MA_GAME_PLAYING_START:
      set_all_insensitive();
      break; 
	case MA_GAME_PLAYING:
      set_all_sensitive();
      break; 
	case MA_WAIT_FOR_SAVE:
      plugin_wait_for_save();
      break; 
	case MA_LOAD_MIME:
      plugin_mime_open();
      break; 
    case MA_GAME_RESET_OLD_SETTINGS:
      plugin_restore_original_settins();
      break;
    case MA_GAME_ENDED:
    case MA_GAME_RESTORE_LAST_SETTINGS:
      plugin_restore_last_settings();
      break;
    case MA_GAME_SAVEMENU_REFERENCE:
      save_item = menu_item;
      break;
   case MA_GAME_BG_KILL:
    if(fc_dialog != NULL)
    {
      gtk_widget_destroy(fc_dialog);
      fc_dialog = NULL;
    }
    break;	
  }
  menu_item=NULL;
}

/* Show a infopint */
static void plugin_show_infoprint(gchar *msg)
{
  
    hildon_banner_show_information( GTK_WIDGET (gs.ui->hildon_appview),NULL, msg);
}

/* Show load error dialog */
static void plugin_ui_show_load_err(int err_type, gchar *loadfile)
{ 
  plugin_settings_set_bool(SETTINGS_CHESS_LOAD_IN_PROGRESS, FALSE);

  switch (err_type) {
    case GAME_LOAD_FILE_UNSUPPORTED: 
    {
      gchar *filename = gnome_vfs_format_uri_for_display(loadfile);
      gchar *basename = g_path_get_basename(filename);
      gchar *temp = g_strrstr(basename, ".");
      if (temp) *temp = '\0';
      gchar *msg = g_strdup_printf(_("game_ni_unsupported_file%s"), basename);

      
      gtk_window_set_title(GTK_WINDOW(gs.ui->hildon_appview), "");  
      plugin_settings_set_string(SETTINGS_CHESS_SAVE_FILE,"");
      plugin_show_infoprint(msg);
      
      g_free(msg);
      g_free(basename);
      g_free(filename);
      break;
    }
    case GAME_LOAD_FILE_NOT_FOUND:
    {
      HildonNote *note = HILDON_NOTE(hildon_note_new_information(
      GTK_WINDOW(gs.ui->hildon_appview), __("sfil_ni_unable_to_open_file_not_found") ));
      hildon_note_set_button_text(note, __("sfil_ni_unable_to_open_file_not_found_ok"));
      /* Rama - changed here */
      fc_dialog = note;
      
      gtk_dialog_run(GTK_DIALOG(note));
      gtk_widget_destroy(GTK_WIDGET(note));

      /* Rama - changed here */
      fc_dialog = NULL;
      break;
    }
  }
}

void game_load(gchar *filename)
{
    StartupApp *app=NULL;
    app=gs.ui->app;
    osso_rpc_t retval;
    osso_return_t ret;
    
    if (!plugin_settings_set_string(SETTINGS_CHESS_SAVE_FILE, filename))
    {
        exit(0);
    }
    gchar *loadfile = g_strdup(filename);
    if (read_handle)
    {
        gnome_vfs_close(read_handle);
        read_handle=NULL;
    }
    GnomeVFSResult result;
    result = gnome_vfs_open (&read_handle, loadfile, GNOME_VFS_OPEN_READ);
    if (result != GNOME_VFS_OK)
    {
        plugin_ui_show_load_err(GAME_LOAD_FILE_NOT_FOUND, loadfile);
        read_handle=NULL;
        return;
    } else {
        gnome_vfs_close(read_handle);
        read_handle=NULL;    
    }

    plugin_settings_set_bool(SETTINGS_CHESS_LOAD_IN_PROGRESS, TRUE);

    /* Validate file */
    if( !validate_chess_file(loadfile) ) {
        gs.startup_ui_state_change_cb(
            gs.ui->app, GAME_CLOSED, GAME_CLOSED, gs.ui);
        plugin_ui_show_load_err(GAME_LOAD_FILE_UNSUPPORTED, loadfile);
        plugin_settings_set_string(SETTINGS_CHESS_SAVE_FILE,"");
    	g_free(loadfile);
    	return;
    }
        

    gchar *tname=NULL;
    tname=comapp_common_get_display_name(filename,NULL,TRUE);
    if (tname)
    {
        gtk_window_set_title(GTK_WINDOW(gs.ui->hildon_appview), tname);	
        g_free(tname);
    }
        
    set_all_insensitive();

    while(gtk_events_pending()) gtk_main_iteration();    

    /* end register to monitoring */ 
    ret=osso_rpc_run(app->osso,
            CHESS_SERVICE,
            CHESS_OBJPATH,
            CHESS_IFACE,
            GAME_LOAD_METHOD,
            &retval,
            0
            );

    if (OSSO_OK==ret)
    {
        if ((retval.type == DBUS_TYPE_BOOLEAN) && (retval.value.b == FALSE)) {
            plugin_ui_show_load_err(GAME_LOAD_FILE_UNSUPPORTED, loadfile);
            if (loadfile != NULL)
                g_free(loadfile);
            osso_rpc_free_val (&retval);
            plugin_settings_set_string(SETTINGS_CHESS_SAVE_FILE,"");
            if (read_handle)
            {
                gnome_vfs_close(read_handle);
                read_handle=NULL;
            }
            gs.startup_ui_state_change_cb(gs.ui->app, GAME_CLOSED,
                    GAME_CLOSED, gs.ui);
            return;
        }
        else {
            plugin_get_settings();
        }
    }
    else {
      plugin_get_settings();
    }

    if (loadfile != NULL)
        g_free(loadfile);
    osso_rpc_free_val (&retval);
    return;
}

/** Callback function when the volume unmounted */
static void plugin_volume_unmounted_cb(GnomeVFSVolumeMonitor *vfsvolumemonitor,
  GnomeVFSVolume *volume, gpointer user_data)
{
  gchar *dst_text = NULL;
  gchar *volume_text = NULL;
  GnomeVFSURI *dst_uri;
  gchar *uri_scheme;
  vfsvolumemonitor=NULL;
  user_data=NULL;


  dst_text = settings_get_string(SETTINGS_CHESS_SAVE_FILE);
  uri_scheme = gnome_vfs_get_uri_scheme(dst_text);
  if (!uri_scheme)
    dst_text = gnome_vfs_get_uri_from_local_path(dst_text);
  else
    g_free(uri_scheme);
  dst_uri = gnome_vfs_uri_new(dst_text);    
  g_free(dst_text);
  
  dst_text = gnome_vfs_uri_to_string(dst_uri, GNOME_VFS_URI_HIDE_NONE);

  if (dst_text != NULL) {
    /* the mount point of the changed volume */
    volume_text = gnome_vfs_volume_get_activation_uri(volume);
  }
  
  /* check if unmounting this volume has anything to do with us */
  if(volume_text && dst_text && (g_str_has_prefix(dst_text, volume_text))) {
    is_mmc = (mmc_uri 
      && g_str_has_prefix(volume_text, mmc_uri));
  }
  
  if (dst_text) g_free (dst_text);
  if (volume_text) g_free(volume_text);
}


/** Show nommc dialog */
static void plugin_ui_show_save_nommc_dialog(void)
{
  HildonNote *note = HILDON_NOTE(hildon_note_new_information(
        GTK_WINDOW(gs.ui->hildon_appview),
        _("ckct_ni_cannot_save_mmc_cover_open")
  	));
  hildon_note_set_button_text(note, _("ckct_bd_cannot_save_mmc_cover_open_ok"));
  
  /* Rama - changed here */
  fc_dialog = note;
  gtk_dialog_run(GTK_DIALOG(note));
  gtk_widget_destroy(GTK_WIDGET(note));

  /* Rama - changed here */
  fc_dialog = NULL;

  gtk_window_set_title(GTK_WINDOW(gs.ui->hildon_appview), "");
  plugin_settings_set_string(SETTINGS_CHESS_SAVE_FILE,"");
}

static void game_save(void)
{
  gdouble cpos = 0.1;
  gchar *tmp_text, *dst_text;
  GnomeVFSURI *tmp_uri, *dst_uri;
  GnomeVFSResult vfs_res;
  gchar *uri_scheme;
  
  /* Set where the MMC is mounted */
  const gchar *mmc_env = NULL;      
  mmc_env = g_getenv(MMC_MOUNTPOINT_ENV);
  if (mmc_env) {
    mmc_uri = g_strconcat (URI_FILE_PREFIX, mmc_env, NULL);
  }

  /* Monitor MMC state */
  guint volume_monitor_id = 0;
  is_mmc = FALSE;
  GnomeVFSVolumeMonitor *volume_monitor;
  volume_monitor = gnome_vfs_get_volume_monitor();
  volume_monitor_id = g_signal_connect(G_OBJECT(volume_monitor), "volume_unmounted", 
    G_CALLBACK(plugin_volume_unmounted_cb), NULL);
  dst_text = settings_get_string(SETTINGS_CHESS_SAVE_FILE);

  /* Checking if there is enough free space */
  struct statfs info;
  gchar *unuried_fname=NULL;
  unuried_fname=gnome_vfs_get_local_path_from_uri(dst_text);
  if (unuried_fname) {
	g_strrstr(unuried_fname,"/")[0]='\0';
	if (statfs(unuried_fname,&info)==0) {
	    GnomeVFSFileInfo *file_info = gnome_vfs_file_info_new();
    	GnomeVFSResult vfs_res = gnome_vfs_get_file_info(CHESS_PAUSE_SAVE_FILE_TMP,
    	  file_info, GNOME_VFS_FILE_INFO_FOLLOW_LINKS);
    	if (vfs_res == GNOME_VFS_OK ||
        	file_info->type == GNOME_VFS_FILE_TYPE_REGULAR) {
      		
      		if (0.02>=((double)info.f_bavail/(double)info.f_blocks)) {
		      	//no more free space
        		g_warning( "Remove this code" );
        		ui_show_nomem_dialog();
        		g_free(unuried_fname);
        		return;
      		}
    	} else {
    		//error while getting the fileinfo, or it is not a regular file
      		plugin_show_infoprint(_("sfil_ni_unable_to_open_file_not_found"));
      		g_free(unuried_fname);
      		return;
    	}
    } else {
  		// error gettint fs info
    	plugin_show_infoprint(unuried_fname);
    	g_free(unuried_fname);
    	return;
  	}
  	g_free(unuried_fname);
  }
  /* Display saving progress dialog */
  plugin_ui_show_saving_dialog();

  if (!ui_set_saving_dialog_progress( cpos)) return;
  chess_save_done=GAME_SAVE_SAVING;
  while(gtk_events_pending())
    gtk_main_iteration();

  
  /* Copy saved file from temp to destination */
  tmp_text = gnome_vfs_get_uri_from_local_path(CHESS_PAUSE_SAVE_FILE_TMP);
        
  
  uri_scheme = gnome_vfs_get_uri_scheme(dst_text);
  if (!uri_scheme)
    dst_text = gnome_vfs_get_uri_from_local_path(dst_text);
  else
    g_free(uri_scheme);

  tmp_uri = gnome_vfs_uri_new(tmp_text);
  dst_uri = gnome_vfs_uri_new(dst_text);

  vfs_res = gnome_vfs_xfer_uri(tmp_uri, dst_uri,
            GNOME_VFS_XFER_DEFAULT,
            GNOME_VFS_XFER_ERROR_MODE_ABORT,
            GNOME_VFS_XFER_OVERWRITE_MODE_REPLACE,
            NULL, NULL);

        
  if (is_mmc) { /* MMC cover open while saving to MMC */
   plugin_ui_hide_saving_dialog();
   while(gtk_events_pending())
     gtk_main_iteration();
        
   plugin_ui_show_save_nommc_dialog();
   chess_saved = FALSE;
   plugin_ui_hide_saving_dialog();
   while(gtk_events_pending())
      gtk_main_iteration();

    while(gtk_events_pending())
     gtk_main_iteration();
    usleep(10000);
    return;
  }

  /* Return if user press Cancel while saving */
  if (chess_save_done==GAME_SAVE_CANCEL) {
     gtk_signal_disconnect(G_OBJECT(volume_monitor), volume_monitor_id);
     return;
  }
  plugin_ui_hide_saving_dialog();
  while(gtk_events_pending())
      gtk_main_iteration();            
  
  /* Copy file error handling */
  switch (vfs_res) {
    case GNOME_VFS_OK:
        plugin_show_infoprint(__("sfil_ib_saved"));
	  	GnomeVFSResult result;
		result = gnome_vfs_open (&read_handle, dst_text, GNOME_VFS_OPEN_READ);
		g_assert(result == GNOME_VFS_OK);
		
		{
    		gchar *tname = comapp_common_get_display_name(dst_text,NULL,TRUE);
    		if (tname) {
        		gtk_window_set_title(GTK_WINDOW(gs.ui->hildon_appview), tname);	
        		g_free(tname);
    		}
    	}
    			
        chess_saved = TRUE;
        break;
        
        
      case GNOME_VFS_ERROR_ACCESS_DENIED:
      case GNOME_VFS_ERROR_READ_ONLY:
      case GNOME_VFS_ERROR_READ_ONLY_FILE_SYSTEM:
		hildon_banner_show_information( GTK_WIDGET(gs.ui->hildon_appview),
			NULL, dgettext("hildon-fm", "sfil_ib_readonly_location" ));      
        chess_saved = FALSE;      	
      	break;
      
      case GNOME_VFS_ERROR_NO_MEMORY:
      case GNOME_VFS_ERROR_NO_SPACE:
      default:
        chess_saved = FALSE;
        ui_show_nomem_dialog();
      }
	gnome_vfs_uri_unref(tmp_uri);
  gnome_vfs_uri_unref(dst_uri);
  g_free(tmp_text);
  g_free(dst_text);
  chess_save_done=GAME_SAVE_NONE;
  gtk_signal_disconnect(G_OBJECT(volume_monitor), volume_monitor_id);
}
  


static void plugin_ui_cancel_saving(void) {
  chess_save_done = GAME_SAVE_CANCEL;
  
  /* wait chess engine return result after saved */ 	  
  while (chess_save_done == GAME_SAVE_CANCEL) {
    while(gtk_events_pending())
      gtk_main_iteration();
    usleep(10000);
  }
  plugin_ui_hide_saving_dialog();  

  /* Was handled by game_save() */
  if (chess_save_done==GAME_SAVE_NONE) {
    return;
  }
  
  /* Delete un-wanted file if file is new */
  if (is_new_file) {
    gchar *filename = settings_get_string(SETTINGS_CHESS_SAVE_FILE);

    gtk_window_set_title(GTK_WINDOW(gs.ui->hildon_appview), "");
    plugin_settings_set_string(SETTINGS_CHESS_SAVE_FILE,"");   	          
  
    if (filename) {
      gnome_vfs_unlink(filename);
      g_free(filename);
    }
    
  }
}



static void plugin_saving_dialog_cb(GtkDialog *dialog, gint button, gpointer user_data)
{
  dialog=NULL;
  user_data=NULL;
  if (button == GTK_RESPONSE_CANCEL) {
    if (chess_save_done == GAME_SAVE_SAVING) {
        plugin_ui_cancel_saving();
    }
  }
}


/** Show the saving dialog */
static void
plugin_ui_show_saving_dialog(void) 
{
  saving_progress_bar=gtk_progress_bar_new();
  chess_saving_dialog=hildon_note_new_cancel_with_progress_bar(GTK_WINDOW(gs.ui->hildon_appview),
  __("sfil_ib_saving"),GTK_PROGRESS_BAR(saving_progress_bar));
  g_signal_connect(G_OBJECT(chess_saving_dialog), "response",
    G_CALLBACK(plugin_saving_dialog_cb), NULL);

  g_signal_connect(G_OBJECT(chess_saving_dialog), "key_press_event",
    G_CALLBACK(plugin_saving_dialog_cb), NULL);
  gtk_widget_show_all(chess_saving_dialog);
}


/** Set the saving dialog progress */
gboolean ui_set_saving_dialog_progress(gfloat progress)
{
  if (chess_saving_dialog==NULL) {
    return FALSE;
  }
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(saving_progress_bar),progress);
  return TRUE;
}


/** Hide the saving dialog */
void plugin_ui_hide_saving_dialog(void)
{
  if (chess_saving_dialog==NULL) return;
  gtk_widget_hide_all(chess_saving_dialog);
  gtk_widget_destroy(GTK_WIDGET(chess_saving_dialog));
  chess_saving_dialog=NULL;
  saving_progress_bar=NULL;
}


/** Show no memory dialog */
void ui_show_nomem_dialog(void)
{
  HildonNote *note = HILDON_NOTE(hildon_note_new_information(
        GTK_WINDOW(gs.ui->hildon_appview),
        __("sfil_ni_not_enough_memory")
  	));
  hildon_note_set_button_text(note, __("sfil_ni_not_enough_memory_ok"));
  
  /* Rama - changed here */
  fc_dialog = note;
  gtk_dialog_run(GTK_DIALOG(note));
  gtk_widget_destroy(GTK_WIDGET(note));
  /* Rama - changed here */
  fc_dialog = NULL;
}


/* Get string type key value */
gchar* settings_get_string(const gchar *key)
{
  return gconf_client_get_string(gcc, key, NULL);
}

/* The following function is from INDT's games-startup code. The copyrights 
belongs to them. The INDT's games-startup was released under LGPL license. */

/** Set the setings on the UI*/
static void plugin_set_settings(void)
{
  gboolean white = TRUE;
  gboolean human = FALSE;
  gboolean sound = TRUE;
  gboolean legal_moves = FALSE;
  gint player = -1;
  gint difficulty = -1;

  
    /* Sound */
    sound = gtk_toggle_button_get_active(
      GTK_TOGGLE_BUTTON( sound_check) );

    /* Human color */
    player = gtk_combo_box_get_active(
      GTK_COMBO_BOX( human_box ) );
    if (player == chess_human_white) white = TRUE;
    else white = FALSE;

    /* Opponent difficulty and type */
    difficulty = gtk_combo_box_get_active(
      GTK_COMBO_BOX( opponent_box ) );
    if (difficulty==5)
    {
      difficulty=4;
    }

    if (difficulty == chess_opponent_human) human = TRUE;
    else human = FALSE;

    /* Show legal moves */
    legal_moves = gtk_toggle_button_get_active(
      GTK_TOGGLE_BUTTON( show_legal_moves_check) );

    /* Set settings */
    plugin_settings_set_bool(SETTINGS_PLAYER_COLOR,
      white);
    plugin_settings_set_bool(SETTINGS_ENABLE_SOUND,
      sound);
    plugin_settings_set_bool(SETTINGS_SHOW_LEGAL_MOVES,
      legal_moves);
    plugin_settings_set_int(SETTINGS_LEVEL,
      difficulty);
    plugin_settings_set_bool(SETTINGS_OPPONENT_HUMAN,
      human);
    
}

static void
file_monitor_cb( GnomeVFSMonitorHandle *handle, const gchar * monitor_uri,
	const gchar *info_uri, GnomeVFSMonitorEventType event_type,
	gpointer user_data) 
{

	/* If file is changed to read only */
    gboolean sensitivity;
	if( event_type == GNOME_VFS_MONITOR_EVENT_METADATA_CHANGED ) {
		GnomeVFSFileInfo info;
		
		if( gnome_vfs_get_file_info( monitor_uri, &info,
			GNOME_VFS_FILE_INFO_GET_ACCESS_RIGHTS ) == GNOME_VFS_OK ) {
		
            if (save_item != NULL) {    
                sensitivity = ( info.permissions &  GNOME_VFS_PERM_ACCESS_WRITABLE ) ==  GNOME_VFS_PERM_ACCESS_WRITABLE;
		if (sensitivity)
		  gtk_widget_show(save_item);
		else
		  gtk_widget_hide(save_item);
            }
		}
	}

	handle = 0;
	info_uri = 0;
	user_data = 0;
}
