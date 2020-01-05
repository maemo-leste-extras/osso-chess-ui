dnl Process this file with autoconf to produce a configure script.
dnl AC_INIT($PACKAGE, $VERSION)
AC_INIT([osso-chess-ui], [1.4.34])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_SRCDIR([src/main.hh])
AC_CONFIG_HEADERS([config.h])

AM_PROG_CC_C_O
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_HEADER_STDC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S

CFLAGS="$CFLAGS -W -Wall"
CXXFLAGS="$CXXFLAGS -W -Wall"

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.2 gnome-vfs-2.0 x11 hildon-1 >= 0.8.1)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(HILDON, hildon-1 >= 0.8.1 hildon-fm-2 >= 0.8.1 libosso >= 0.8.1 comapp)
AC_SUBST(HILDON_CFLAGS)
AC_SUBST(HILDON_LIBS)

PKG_CHECK_MODULES(GCONF, gconf-2.0 >= 2.2.0)
AC_SUBST(GCONF_CFLAGS)
AC_SUBST(GCONF_LIBS)

PKG_CHECK_MODULES(OSSO, libosso >= 0.7.3)
AC_SUBST(OSSO_CFLAGS)
AC_SUBST(OSSO_LIBS)

PKG_CHECK_MODULES(PNG, libpng >= 1.2.0)
AC_SUBST(PNG_CFLAGS)
AC_SUBST(PNG_LIBS)

PKG_CHECK_MODULES(PANGO, pango >= 1.6.0)
AC_SUBST(PANGO_CFLAGS)
AC_SUBST(PANGO_LIBS)

PKG_CHECK_MODULES(CANBERRA, libcanberra)
AC_SUBST(CANBERRA_CFLAGS)
AC_SUBST(CANBERRA_LIBS)

GETTEXT_PACKAGE=osso-games
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED( GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Name of gettext package] )
ALL_LINGUAS="de_DE en_GB en_US es_ES es_MX fi_FI fr_CA fr_FR it_IT pt_BR ru_RU"
AM_GLIB_GNU_GETTEXT
AC_PROG_LIBTOOL

dnl MIME-type registration check (2.6 / 2.8 version)
AC_PATH_PROG(UPDATE_MIME_DATABASE, update-mime-database, no)
AM_CONDITIONAL(HAVE_26_MIME, test x$UPDATE_MIME_DATABASE != xno)

AC_PATH_PROG(UPDATE_DESKTOP_DATABASE, update-desktop-database, no)
AM_CONDITIONAL(HAVE_28_MIME, test x$UPDATE_DESKTOP_DATABASE != xno)

pixmapsdir=${datadir}/pixmaps/osso-chess/
AC_SUBST(pixmapsdir)

soundsdir=${datadir}/sounds/
AC_SUBST(soundsdir)

chessuilocaledir=${datadir}/locale
AC_SUBST(chessuilocaledir)

desktopentrydir=/usr/share/applications
AC_SUBST(desktopentrydir)

mimedir=/usr/share/mime/packages
AC_SUBST(mimedir)

hildondesktopentrydir=${desktopentrydir}/hildon
hildoncpdesktopentrydir=`$PKG_CONFIG --variable=desktopentrydir hildon-control-panel`
hildonpixmapdir=`$PKG_CONFIG --variable=hildonpixmapdir hildon-base`
hildonstatusbarlibdir=`$PKG_CONFIG --variable=hildonstatusbarlibdir hildon-base`

AC_SUBST(hildondesktopentrydir)
AC_SUBST(hildoncpdesktopentrydir)
AC_SUBST(hildonpixmapdir)
AC_SUBST(hildonstatusbarlibdir)

dbuslibpathdir=`pkg-config --variable=libdir dbus-1`
dbusconfdir=/etc/dbus-1/system.d
dbusservdir=${dbuslibpathdir}/dbus-1.0/services
AC_SUBST(dbuslibpathdir)
AC_SUBST(dbusconfdir)
AC_SUBST(dbusservdir)

AC_CONFIG_FILES(
Makefile
src/Makefile
src/ui/Makefile
src/babychess/Makefile
src/chess_startup.desktop
src/com.nokia.osso_chess.service
src/com.nokia.osso_chess.startup.service
src/osso-chess-ui.xml
src/plugin/Makefile)

AC_OUTPUT
