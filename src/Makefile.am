SUBDIRS = babychess ui plugin
INCLUDES = $(GTK_CFLAGS) $(OSSO_CFLAGS) $(GCONF_CFLAGS) $(PNG_CFLAGS) \
	   $(PANGO_CFLAGS) $(ESOUND_CFLAGS)
	   
CC=g++

servicefiledir=$(prefix)/share/dbus-1/services
servicefile_DATA = com.nokia.osso_chess.service \
		   com.nokia.osso_chess.startup.service

configdir = $(datadir)/osso-chess-ui
config_DATA = osso_chess.conf

backupconfigurationdir=/etc/osso-backup/applications
backupconfiguration_DATA=osso_chess_backup.conf

hildondesktopentry_DATA = chess_startup.desktop

mime_DATA = osso-chess-ui.xml

bin_PROGRAMS = chessui


edit = sed \
	-e 's,@datadir\@,$(datadir),g' \
	-e 's,@prefix\@,$(prefix),g' \
	-e 's,@VERSION\@,$(VERSION),g'

osso_chess.conf: osso_chess.conf.in
	@$(edit) $< > $@


chessui_CFLAGS = $(INCLUDES) \
		-DLOCALEDIR=\"$(chessuilocaledir)\" \
		-DPIXMAPSDIR=\"$(pixmapsdir)\" \
		-DSOUNDSDIR=\"$(soundsdir)\" \
                -DPREFIX=\"$(prefix)\" -I$(top_srcdir) \
			$(MAEMO_LAUNCHER_CFLAGS)
chessui_CXXFLAGS = $(chessui_CFLAGS)

chessui_SOURCES = \
	main.cc main.hh \
	dbus.c dbus.h \
	settings.c settings.h \
	state_save.c state_save.h \
	chess_communication.cc chess_communication.hh \
	chess_enginecommunication.cc chess_enginecommunication.hh \
	chess_core.cc chess_core.hh \
	chess_timing.cc chess_timing.hh \
	appdata.hh chessui_i18n.hh \
	sound.c sound.h \
	sounds.h

chessui_LDFLAGS = -Wl,--version-script=export.map

UNWANTED = -export-dynamic \
           -Wl,--export-dynamic

chessui_LDADD = \
	$(filter-out $(UNWANTED), \
	$(MAEMO_LAUNCHER_LDFLAGS) \
	$(GTK_LIBS)   \
       	$(OSSO_LIBS)  \
	$(GCONF_LIBS) \
	$(PNG_LIBS)   \
	$(PANGO_LIBS) \
	$(ESOUND_LIBS) \
	ui/libchessui.a \
	babychess/libbabychess.a)
