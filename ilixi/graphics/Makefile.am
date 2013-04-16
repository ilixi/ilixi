## Makefile.am for /ilixi/graphics
INCLUDES						= 	-I$(top_srcdir)/ilixi -I$(top_builddir)/ilixi
noinst_LTLIBRARIES 				= 	libilixi_graphics.la
libilixi_graphics_la_CPPFLAGS 	= 	$(AM_CPPFLAGS) @DEPS_CFLAGS@ 
libilixi_graphics_la_CFLAGS		= 	$(AM_CFLAGS)
libilixi_graphics_la_LIBADD 	= 	@DEPS_LIBS@
libilixi_graphics_la_SOURCES 	= 	FontPack.cpp \
									IconPack.cpp \
									Painter.cpp \
                  					Palette.cpp \
                  					Style.cpp \
                  					Stylist.cpp \
                  					StylistBase.cpp \
                  					Surface.cpp
          					
ilixi_includedir 				= 	$(includedir)/$(PACKAGE)-$(VERSION)/graphics
nobase_ilixi_include_HEADERS 	= 	FontPack.h \
									IconPack.h \
									Painter.h \
                  					Palette.h \
                  					Style.h \
                  					Stylist.h \
                  					StylistBase.h \
                  					Surface.h

if WITH_REFLEX
GRAPHICS_REFLEX					= 	Painter_rflx.cpp \
                  					Palette_rflx.cpp \
                  					Style_rflx.cpp \
                  					Stylist_rflx.cpp \
                  					StylistBase_rflx.cpp \
                  					Surface_rflx.cpp
                  					
nodist_libilixi_graphics_la_SOURCES = $(GRAPHICS_REFLEX)

include $(top_srcdir)/ilixi/gccxml.make

clean-local:
	-rm $(GRAPHICS_REFLEX)
endif