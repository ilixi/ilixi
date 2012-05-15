## Makefile.am for bin/compositor
SUBDIRS			=	images
bin_PROGRAMS 		= 	ilixi_comp
ilixi_comp_LDADD	=	@DEPS_LIBS@ $(top_srcdir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_comp_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_comp_CFLAGS	=	$(AM_CFLAGS)
ilixi_comp_SOURCES	= 	AppCompositor.h \
					AppCompositor.cpp \
					AppThumbnail.h \
					AppThumbnail.cpp \
					AppView.h \
					AppView.cpp \
					Compositor.cpp \
					Compositor.h \
					Switcher.h \
					Switcher.cpp \
					Launcher.h \
					Launcher.cpp \
					LauncherButton.h \
					LauncherButton.cpp \
					HomeButton.h \
					HomeButton.cpp \
					QuitButton.h \
					QuitButton.cpp \
					SwitchButton.h \
					SwitchButton.cpp\
					types/AppInstance.h \
					types/AppInstance.cpp \
					types/AppInfo.h \
					types/AppInfo.cpp \
					ApplicationManager.h \
					ApplicationManager.cpp