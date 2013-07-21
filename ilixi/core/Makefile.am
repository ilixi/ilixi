## Makefile.am for /ilixi/core
INCLUDES 					= 	-I$(top_srcdir)/ilixi -I$(top_builddir)/ilixi
noinst_LTLIBRARIES 			= 	libilixi_core.la
libilixi_core_la_CPPFLAGS	= 	$(AM_CPPFLAGS) @DEPS_CFLAGS@ $(AM_CFLAGS)
libilixi_core_la_CFLAGS		= 	$(AM_CFLAGS)
libilixi_core_la_LIBADD 	= 	@DEPS_LIBS@
libilixi_core_la_SOURCES 	= 	Application.cpp \
								Callback.cpp \
								Engine.cpp \
	     						EventManager.cpp \
	     						Logger.cpp \
	     						PlatformManager.cpp \
	     						Service.cpp \
	     						Window.cpp
						
ilixi_includedir			= 	$(includedir)/$(PACKAGE)-$(VERSION)/core
ilixi_include_HEADERS		=	Application.h \
								Callback.h \
								Engine.h \
	     						EventManager.h \
	     						Logger.h \
	     						PlatformManager.h \
	     						Service.h \
	     						Window.h

if WITH_FUSIONDALE
libilixi_core_la_SOURCES 	+= 	ComaComponent.cpp \
								DaleDFB.cpp
ilixi_include_HEADERS		+=	ComaComponent.h \
								ComponentData.h \
								DaleDFB.h
endif

if WITH_SURFACE_EVENTS
libilixi_core_la_SOURCES 	+= 	SurfaceEventListener.cpp
ilixi_include_HEADERS		+=	SurfaceEventListener.h
endif

libilixi_core_la_SOURCES 	+= 	SoundDFB.cpp
ilixi_include_HEADERS		+=	SoundDFB.h

if WITH_REFLEX
CORE_REFLEX					= 	Application_rflx.cpp \
								Callback_rflx.cpp \
	     						EventManager_rflx.cpp \
	     						Logger_rflx.cpp \
	     						SurfaceEventListener_rflx.cpp \
	     						Window_rflx.cpp
	     						
nodist_libilixi_core_la_SOURCES = $(CORE_REFLEX)

include $(top_srcdir)/ilixi/gccxml.make

clean-local:
	-rm $(CORE_REFLEX)
endif