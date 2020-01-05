plugindir = @prefix@/lib/osso-chess-ui
plugin_PROGRAMS = chess_plugin.so

chess_plugin_so_CFLAGS = $(GTK_CFLAGS) $(GCONF_CFLAGS) $(GNOMEVFS_CFLAGS) $(GLIB_CFLAGS) $(HILDON_CFLAGS) $(OSSO_CFLAGS) \
	-DCHESSLOCALEDIR=\"$(datadir)/locale\" -fPIC
	
chess_plugin_so_CXXFLAGS = $(chess_plugin_so_CFLAGS)

chess_plugin_so_LDFLAGS = -shared

chess_plugin_so_LDADD = $(GTK_LIBS) $(OSSO_LIBS) $(GCONF_LIBS) $(HILDON_LIBS) \
	$(PANGO_LIBS)

chess_plugin_so_SOURCES = plugin.c

