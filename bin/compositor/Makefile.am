## Makefile.am for bin/compositor
SUBDIRS				=	images
bin_PROGRAMS 		= 	ilixi_comp
ilixi_comp_LDADD	=	@DEPS_LIBS@ $(top_srcdir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_comp_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@ -Ibuttons -Iswitcher -Itypes
ilixi_comp_CFLAGS	=	$(AM_CFLAGS)
ilixi_comp_SOURCES	= 	AppCompositor.h \
						AppCompositor.cpp \
						ApplicationManager.h \
						ApplicationManager.cpp \
						AppThumbnail.h \
						AppThumbnail.cpp \
						AppView.h \
						AppView.cpp \
						Compositor.cpp \
						Compositor.h \
						Launcher.h \
						Launcher.cpp \
						LauncherButton.h \
						LauncherButton.cpp \
						buttons/HomeButton.h \
						buttons/HomeButton.cpp \
						buttons/QuitButton.h \
						buttons/QuitButton.cpp \
						buttons/SwitchButton.h \
						buttons/SwitchButton.cpp \
						switcher/CarouselSwitcher.h \
						switcher/CarouselSwitcher.cpp \
						switcher/HorizontalSwitcher.h \
						switcher/HorizontalSwitcher.cpp \
						switcher/Switcher.h \
						switcher/Switcher.cpp \
						types/AppInfo.h \
						types/AppInfo.cpp \
						types/AppInstance.h \
						types/AppInstance.cpp
						