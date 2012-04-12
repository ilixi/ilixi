## Makefile.am for bin/comp
SUBDIRS			=	images
bin_PROGRAMS 		= 	ilixi_comp
ilixi_comp_LDADD	=	@DEPS_LIBS@ $(top_srcdir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la  $(AM_LDFLAGS)
ilixi_comp_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) @DEPS_CFLAGS@ $(AM_CFLAGS)
ilixi_comp_SOURCES	= 	Compositor.cpp \
					Compositor.h \
					CWindow.cpp \
					CWindow.h \
					Switcher.h \
					Switcher.cpp \
					Launcher.h \
					Launcher.cpp \
					AppButton.h \
					AppButton.cpp \
					HomeButton.h \
					HomeButton.cpp \
					SwitchButton.h \
					SwitchButton.cpp \
					CompositorSurfaceView.h \
					CompositorSurfaceView.cpp
