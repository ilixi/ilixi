## Makefile.am for /ilixi/core
INCLUDES 			= 	-I$(top_srcdir)/ilixi
noinst_LTLIBRARIES 		= 	libilixi_core.la
libilixi_core_la_CPPFLAGS 	= 	@DEPS_CFLAGS@
libilixi_core_la_LIBADD 	= 	@DEPS_LIBS@
libilixi_core_la_SOURCES 	= 	AppBase.cpp \
					Callback.cpp \
					EventManager.cpp \
					IFusion.cpp \
					Logger.cpp \
					SurfaceEventListener.cpp \
					Thread.cpp \
					Window.cpp
ilixi_includedir		= 	$(includedir)/$(PACKAGE)-$(VERSION)/core
ilixi_include_HEADERS		=	AppBase.h \
					Callback.h \
					EventManager.h \
					IFusion.h \
					Logger.h \
					SurfaceEventListener.h \
					Thread.h \
					Window.h
