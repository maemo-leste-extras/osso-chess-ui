# This variable tells which non-source files should be included in
# "make dist"
EXTRA_DIST = \
	autogen.sh \
	debian/changelog \
	debian/compat \
	debian/control \
	debian/copyright \
	debian/docs \
	debian/osso-chess-ui.links \
	debian/rules

SUBDIRS = src

INCLUDES = $(GTK_CFLAGS) $(OSSO_CFLAGS) $(GCONF_CFLAGS) $(PNG_CFLAGS)

deb:	dist
	-mkdir $(top_builddir)/debian-build
	cd $(top_builddir)/debian-build && tar zxf ../$(top_builddir)/$(PACKAGE)-$(VERSION).tar.gz
	cd $(top_builddir)/debian-build/$(PACKAGE)-$(VERSION) && dpkg-buildpackage -rfakeroot -uc -us -sa -D
	-rm -rf $(top_builddir)/debian-build/$(PACKAGE)-$(VERSION)
