INCLUDES = $(GTK_CFLAGS) $(GCONF_CFLAGS) $(OSSO_CFLAGS) $(PANGO_CFLAGS) $(MAEMO_LAUNCHER_CFLAGS)

CC=g++

lib_LIBRARIES = libchessui.a

libchessui_a_CFLAGS = $(INCLUDES) \
		-DLOCALEDIR=\"$(chessuilocaledir)\" \
		-DPIXMAPSDIR=\"$(pixmapsdir)\" \
                -DPREFIX=\"$(prefix)\" -I$(top_srcdir) -I.. -I../babychess
libchessui_a_CXXFLAGS = $(libchessui_a_CFLAGS)

libchessui_a_SOURCES = \
	interface.cc interface.hh \
	callbacks.cc callbacks.hh \
	chess_button.cc chess_button.hh \
	chess_status.cc chess_status.hh \
	chess_moves_list.cc chess_moves_list.hh
