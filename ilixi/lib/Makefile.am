## Makefile.am for /ilixi/lib
INCLUDES 				= 	-I$(top_srcdir)/ilixi
noinst_LTLIBRARIES 		= 	libilixi_lib.la
libilixi_lib_la_CPPFLAGS= 	$(AM_CPPFLAGS) @DEPS_CFLAGS@ 
libilixi_lib_la_CFLAGS	= 	$(AM_CFLAGS)
libilixi_lib_la_LIBADD	= 	@DEPS_LIBS@
libilixi_lib_la_SOURCES = 	Animation.cpp \
							Easing.cpp \
							FPSCalculator.cpp \
							Tween.cpp \
							TweenAnimation.cpp \
							Timer.cpp

ilixi_includedir		= 	$(includedir)/$(PACKAGE)-$(VERSION)/lib
ilixi_include_HEADERS	=	Animation.h \
							Easing.h \
							FPSCalculator.h \
							Tween.h \
							TweenAnimation.h \
							Timer.h
							