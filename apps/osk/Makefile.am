## Makefile.am for apps/osk
SUBDIRS				=	images
bin_PROGRAMS 		= 	ilixi_osk
ilixi_osk_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la  $(AM_LDFLAGS)
ilixi_osk_CFLAGS	=	$(AM_CFLAGS)
ilixi_osk_CPPFLAGS 	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_osk_SOURCES	= 	Helper.cpp \
						Helper.h \
						Key.cpp \
						Key.h \
						Keyboard.cpp \
						Keyboard.h \
						Row.cpp \
						Row.h \
						OSK.cpp \
						OSK.h \
						utf8-decoder.h \
						utf8-decoder.cpp
						
miscdatadir 	=	$(ILIXI_DATADIR)/osk
miscdata_DATA 	=	keyboard.xml keyboard.dtd numpad.xml
EXTRA_DIST 		= 	$(miscdata_DATA)
						