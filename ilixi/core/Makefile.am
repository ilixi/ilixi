## Makefile.am for /ilixi/core
INCLUDES 					= 	-I$(top_srcdir)/ilixi
noinst_LTLIBRARIES 			= 	libilixi_core.la
libilixi_core_la_CPPFLAGS	= 	$(AM_CPPFLAGS) @DEPS_CFLAGS@ $(AM_CFLAGS)
libilixi_core_la_CFLAGS		= 	$(AM_CFLAGS)
libilixi_core_la_LIBADD 		= 	@DEPS_LIBS@
libilixi_core_la_SOURCES 	= 	AppBase.cpp \
     						Callback.cpp \
     						ComaComponent.cpp \
     						EventManager.cpp \
     						Logger.cpp \
     						SurfaceEventListener.cpp \
     						Thread.cpp \
     						Window.cpp
						
ilixi_includedir			= 	$(includedir)/$(PACKAGE)-$(VERSION)/core
ilixi_include_HEADERS		=	AppBase.h \
     						Callback.h \
     						ComaComponent.h \
     						EventManager.h \
     						Logger.h \
     						SurfaceEventListener.h \
     						Thread.h \
     						Window.h
