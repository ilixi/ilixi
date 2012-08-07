## Makefile.am for bin/statusbar
SUBDIRS						= 	images
bin_PROGRAMS 				= 	ilixi_statusbar
ilixi_statusbar_LDADD		=	@DEPS_LIBS@ $(top_srcdir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_statusbar_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_statusbar_CFLAGS		=	$(AM_CFLAGS)
ilixi_statusbar_SOURCES		= 	StatusBar.cpp \
								StatusBar.h \
								StatusbarComponent.cpp \
								StatusbarComponent.h \
								Clock.h \
								Clock.cpp
