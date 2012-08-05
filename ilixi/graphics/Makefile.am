## Makefile.am for /ilixi/graphics
INCLUDES					= 	-I$(top_srcdir)/ilixi
noinst_LTLIBRARIES 			= 	libilixi_graphics.la
libilixi_graphics_la_CPPFLAGS = 	$(AM_CPPFLAGS) @DEPS_CFLAGS@ 
libilixi_graphics_la_CFLAGS	= 	$(AM_CFLAGS)
libilixi_graphics_la_LIBADD 	= 	@DEPS_LIBS@
libilixi_graphics_la_SOURCES 	= 	Painter.cpp \
          					Palette.cpp \
          					Style.cpp \
          					Stylist.cpp \
          					StylistBase.cpp \
          					Surface.cpp
          					
ilixi_includedir 			= 	$(includedir)/$(PACKAGE)-$(VERSION)/graphics
nobase_ilixi_include_HEADERS 	= 	Painter.h \
          					Palette.h \
          					Style.h \
          					Stylist.h \
          					StylistBase.h \
          					Surface.h
