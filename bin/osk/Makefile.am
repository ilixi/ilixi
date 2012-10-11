## Makefile.am for bin/osk
SUBDIRS				=	images
bin_PROGRAMS 		= 	ilixi_osk
ilixi_osk_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la  $(AM_LDFLAGS)
ilixi_osk_CFLAGS	=	$(AM_CFLAGS)
ilixi_osk_CPPFLAGS 	= 	-I$(top_srcdir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_osk_SOURCES	= 	Key.cpp \
						Key.h \
						Keyboard.cpp \
						Keyboard.h \
						Row.cpp \
						Row.h \
						OSK.cpp \
						OSK.h \
						utf8-decoder.h
						
miscdatadir 	=	$(ILIXI_DATADIR)/osk
miscdata_DATA 	=	keyboard.xml keyboard.dtd
EXTRA_DIST 		= 	$(miscdata_DATA)
						