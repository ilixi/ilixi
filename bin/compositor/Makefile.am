## Makefile.am for bin/compositor
SUBDIRS				=	images
bin_PROGRAMS 		= 	ilixi_comp
ilixi_comp_LDADD	=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_comp_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@ -I$(srcdir)/buttons -I$(srcdir)/switcher -I$(srcdir)/types
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
						component/CompositorComponent.h \
						component/CompositorComponent.cpp \
						component/Notification.h \
						component/Notification.cpp \
						component/NotificationManager.h \
						component/NotificationManager.cpp \
						component/OSKComponent.cpp \
						component/OSKComponent.h \
						component/SoundComponent.h \
						component/SoundComponent.cpp \
						switcher/CarouselSwitcher.h \
						switcher/CarouselSwitcher.cpp \
						switcher/HorizontalScroller.cpp \
						switcher/HorizontalScroller.h \
						switcher/HorizontalSwitcher.h \
						switcher/HorizontalSwitcher.cpp \
						switcher/Switcher.h \
						switcher/Switcher.cpp \
						types/AppInfo.h \
						types/AppInfo.cpp \
						types/AppInstance.h \
						types/AppInstance.cpp
						