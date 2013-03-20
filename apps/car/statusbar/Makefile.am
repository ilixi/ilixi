## Makefile.am for apps/statusbar
SUBDIRS						= 	images
bin_PROGRAMS 				= 	ilixi_carstatus
ilixi_carstatus_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_carstatus_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_carstatus_CFLAGS		=	$(AM_CFLAGS)
ilixi_carstatus_SOURCES		= 	Clock.h \
								Clock.cpp \
								RemoteContentArea.cpp \
								RemoteContentArea.h \
								StatusBar.cpp \
								StatusBar.h \
								StatusbarButton.cpp \
								StatusbarButton.h \
								StatusbarComponent.cpp \
								StatusbarComponent.h
