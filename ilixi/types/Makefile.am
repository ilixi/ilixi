## Makefile.am for /ilixi/types
INCLUDES 					= 	-I$(top_srcdir)/ilixi -I$(top_builddir)/ilixi
noinst_LTLIBRARIES 			= 	libilixi_types.la
libilixi_types_la_CPPFLAGS 	= 	$(AM_CPPFLAGS) @DEPS_CFLAGS@
libilixi_types_la_CFLAGS 	= 	$(AM_CFLAGS)
libilixi_types_la_LIBADD 	= 	@DEPS_LIBS@
libilixi_types_la_SOURCES 	=	Affine2D.cpp \
	          					Brush.cpp \
	          					Color.cpp \
	          					Event.cpp \
	          					Font.cpp \
	          					FontCache.cpp \
	          					Image.cpp \
	          					Margin.cpp \
	          					Pen.cpp \
	          					Point.cpp \
	          					RadioGroup.cpp \
	          					Rectangle.cpp \
	          					Size.cpp \
	          					TextLayout.cpp \
	          					Video.cpp

ilixi_includedir 				= 	$(includedir)/$(PACKAGE)-$(VERSION)/types
nobase_ilixi_include_HEADERS 	= 	Affine2D.h \
		          					Brush.h \
		          					Color.h \
		          					Enums.h \
		          					Event.h \
		          					Font.h \
		          					FontCache.h \
		          					Image.h \
		          					Margin.h \
		          					Pen.h \
		          					Point.h \
		          					RadioGroup.h \
		          					Rectangle.h \
		          					Size.h \
		          					TextLayout.h \
		          					Video.h

if WITH_FUSIONSOUND
libilixi_types_la_SOURCES 		+= 	Music.cpp \
	          						Sound.cpp
nobase_ilixi_include_HEADERS	+=	Music.h \
									Sound.h
endif

if WITH_NLS
libilixi_types_la_SOURCES 		+= 	I18NBase.cpp
nobase_ilixi_include_HEADERS	+=	I18NBase.h
endif

if WITH_REFLEX
TYPES_REFLEX				=	Affine2D_rflx.cpp \
	          					Brush_rflx.cpp \
	          					Color_rflx.cpp \
	          					Event_rflx.cpp \
	          					Font_rflx.cpp \
	          					FontCache_rflx.cpp \
	          					Image_rflx.cpp \
	          					Margin_rflx.cpp \
	          					Pen_rflx.cpp \
	          					Point_rflx.cpp \
	          					RadioGroup_rflx.cpp \
	          					Rectangle_rflx.cpp \
	          					Size_rflx.cpp

nodist_libilixi_types_la_SOURCES = $(TYPES_REFLEX)

include $(top_srcdir)/ilixi/gccxml.make

clean-local:
	-rm $(TYPES_REFLEX)
endif