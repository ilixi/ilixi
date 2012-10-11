## Makefile.am for bin/statusbar
SUBDIRS						= 	images
bin_PROGRAMS 				= 	ilixi_statusbar
ilixi_statusbar_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_statusbar_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_statusbar_CFLAGS		=	$(AM_CFLAGS)
ilixi_statusbar_SOURCES		= 	Clock.h \
								Clock.cpp \
								RemoteContentArea.cpp \
								RemoteContentArea.h \
								StatusBar.cpp \
								StatusBar.h \
								StatusbarButton.cpp \
								StatusbarButton.h \
								StatusbarComponent.cpp \
								StatusbarComponent.h
