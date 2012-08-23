## Makefile.am for /ilixi/lib
INCLUDES 				= 	-I$(top_srcdir)/ilixi
noinst_LTLIBRARIES 		= 	libilixi_lib.la
libilixi_lib_la_CPPFLAGS= 	$(AM_CPPFLAGS) @DEPS_CFLAGS@ 
libilixi_lib_la_CFLAGS	= 	$(AM_CFLAGS)
libilixi_lib_la_LIBADD	= 	@DEPS_LIBS@
libilixi_lib_la_SOURCES = 	Animation.cpp \
							AnimationSequence.cpp \
							Easing.cpp \
							FileSystem.cpp \
							FPSCalculator.cpp \
							Thread.cpp \
							Timer.cpp \
							Tween.cpp \
							TweenAnimation.cpp \
							Util.cpp

ilixi_includedir		= 	$(includedir)/$(PACKAGE)-$(VERSION)/lib
ilixi_include_HEADERS	=	Animation.h \
							AnimationSequence.h \
							Easing.h \
							FileSystem.h \
							FPSCalculator.h \
							Thread.h \
							Timer.h \
							Tween.h \
							TweenAnimation.h \
							Util.h

if WITH_FUSIONDALE
libilixi_lib_la_SOURCES 	+= 	Notify.cpp
ilixi_include_HEADERS		+=	Notify.h
endif

if WITH_FUSIONSOUND
libilixi_lib_la_SOURCES 	+= 	Sound.cpp
ilixi_include_HEADERS		+=	Sound.h
endif