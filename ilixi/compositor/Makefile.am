## Makefile.am for /ilixi/compositor
INCLUDES						= 	-I$(top_srcdir)/ilixi -I$(top_builddir)/ilixi
noinst_LTLIBRARIES 				= 	libilixi_compositor.la
libilixi_compositor_la_CPPFLAGS = 	$(AM_CPPFLAGS) @DEPS_CFLAGS@ 
libilixi_compositor_la_CFLAGS	= 	$(AM_CFLAGS)
libilixi_compositor_la_LIBADD 	= 	@DEPS_LIBS@
libilixi_compositor_la_SOURCES 	= 	AppCompositor.cpp \
									AppInfo.cpp \
									AppInstance.cpp \
									ApplicationManager.cpp \
									AppThumbnail.cpp \
									AppView.cpp \
									Compositor.cpp \
									CompositorComponent.cpp \
									MemoryMonitor.cpp \
									Notification.cpp \
									NotificationManager.cpp \
									OSKComponent.cpp \
									SoundComponent.cpp \
									Switcher.cpp
          					
ilixi_includedir 				= 	$(includedir)/$(PACKAGE)-$(VERSION)/compositor
nobase_ilixi_include_HEADERS 	= 	AppCompositor.h \
									AppInfo.h \
									AppInstance.h \
									ApplicationManager.h \
									AppThumbnail.h \
									AppView.h \
									Compositor.h \
									CompositorComponent.h \
									MemoryMonitor.h \
									Notification.h \
									NotificationManager.h \
									OSKComponent.h \
									SoundComponent.h \
									Switcher.h
		