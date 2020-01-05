/**
    @file main.c

    Application main.

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

#include <libgnomevfs/gnome-vfs.h>
#include "main.hh"
#include "sound.h"
#include <hildon/hildon.h>


/**
 Handles signals, ensure everything gets deinitted
 @param sig Signal
 */
void sig_handler(int sig)
{
  /*XXX g_print("Signal: %d\n",sig);
  osso_log(LOG_DEBUG,"Signal: %d",sig);*/
  if (sig==SIGINT) {
    chess_end();
    chess_close(TRUE);
  }
}

int main(int argc, char** argv)
{
  AppData *app_data;
//  uint level = 0;

  ULOG_OPEN(PACKAGE_NAME " " PACKAGE_VERSION);

  setlocale (LC_ALL, "");
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  hildon_gtk_init(&argc,&argv);

  osso_log(LOG_DEBUG,"Chess start");

  app_data = g_new0( AppData, 1 );
  app_data->app_ui_data = g_new0( AppUIData, 1 );
  app_data->app_osso_data = g_new0( AppOSSOData, 1 );
  app_data->app_gconf_data = g_new0( AppGConfData, 1 );
  app_data->app_sound_data = g_new0( AppSoundData, 1 );

  /* Init GConf */
  if (!init_settings(app_data)) {
    osso_log(LOG_ERR,"GConf initialization failed");
    puts("GConf initialization failed\n");
    return 1;
  }

  if (!gnome_vfs_initialized()) {
    if (!gnome_vfs_init()) {
      osso_log(LOG_ERR, "Could not init GnomeVFS");
      return 1;
    } 
  }
  
  /* Init osso */
  if (!init_osso(app_data)) {
    osso_log(LOG_ERR,"Osso initialization failed");
    puts("Osso initialization failed\n");
    return 1;
  }
  
  /* Get settings from GConf */
  get_chess_settings(app_data);
  
  g_idle_add_full(G_PRIORITY_HIGH,ui_create_main_window,(void *)app_data,NULL);

  /* Set handler for keyboard interrupt */
  signal(SIGINT,sig_handler);

  if (app_data->app_ui_data->enable_sound)
    sound_init(app_data->app_sound_data);

  gtk_main();

  if (gnome_vfs_initialized()) {
    gnome_vfs_shutdown();
  }
  deinit_engine();
  deinit_osso(app_data);

  LOG_CLOSE();

  return 0;
}
