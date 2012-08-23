## Makefile.am for /ilixi/core
INCLUDES 					= 	-I$(top_srcdir)/ilixi
noinst_LTLIBRARIES 			= 	libilixi_core.la
libilixi_core_la_CPPFLAGS	= 	$(AM_CPPFLAGS) @DEPS_CFLAGS@ $(AM_CFLAGS)
libilixi_core_la_CFLAGS		= 	$(AM_CFLAGS)
libilixi_core_la_LIBADD 	= 	@DEPS_LIBS@
libilixi_core_la_SOURCES 	= 	AppBase.cpp \
     							Callback.cpp \
	     						EventManager.cpp \
	     						Logger.cpp \
	     						SurfaceEventListener.cpp \
	     						Window.cpp
						
ilixi_includedir			= 	$(includedir)/$(PACKAGE)-$(VERSION)/core
ilixi_include_HEADERS		=	AppBase.h \
	     						Callback.h \
	     						EventManager.h \
	     						Logger.h \
	     						SurfaceEventListener.h \
	     						Window.h

if WITH_FUSIONDALE
libilixi_core_la_SOURCES 	+= 	ComaComponent.cpp
ilixi_include_HEADERS		+=	ComaComponent.h
endif