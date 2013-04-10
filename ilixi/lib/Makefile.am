## Makefile.am for /ilixi/lib
INCLUDES 				= 	-I$(top_srcdir)/ilixi -I$(top_builddir)/ilixi 
noinst_LTLIBRARIES 		= 	libilixi_lib.la
libilixi_lib_la_CPPFLAGS= 	$(AM_CPPFLAGS) @DEPS_CFLAGS@ 
libilixi_lib_la_CFLAGS	= 	$(AM_CFLAGS)
libilixi_lib_la_LIBADD	= 	@DEPS_LIBS@
libilixi_lib_la_SOURCES = 	Animation.cpp \
							AnimationSequence.cpp \
							Easing.cpp \
							FileInfo.cpp \
							FileSystem.cpp \
							FPSCalculator.cpp \
							InputHelper.cpp \
							InputHelperJP.cpp \
							Thread.cpp \
							Timer.cpp \
							Tween.cpp \
							TweenAnimation.cpp \
							utf8.c \
							Util.cpp \
							XMLReader.cpp

ilixi_includedir		= 	$(includedir)/$(PACKAGE)-$(VERSION)/lib
ilixi_include_HEADERS	=	Animation.h \
							AnimationSequence.h \
							Easing.h \
							FileInfo.h \
							FileSystem.h \
							FPSCalculator.h \
							InputHelper.h \
							InputHelperJP.h \
							Thread.h \
							Timer.h \
							Tween.h \
							TweenAnimation.h \
							utf8.h \
							Util.h \
							XMLReader.h

if WITH_FUSIONDALE
libilixi_lib_la_SOURCES 	+= 	Notify.cpp
ilixi_include_HEADERS		+=	Notify.h
endif

if WITH_REFLEX
nodist_libilixi_lib_la_SOURCES 	= UIBuilder.cpp
ilixi_include_HEADERS		+=	UIBuilder.h
endif
