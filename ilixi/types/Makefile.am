## Makefile.am for /ilixi/types
INCLUDES 					= 	-I$(top_srcdir)/ilixi
noinst_LTLIBRARIES 			= 	libilixi_types.la
libilixi_types_la_CPPFLAGS 	= 	$(AM_CPPFLAGS) @DEPS_CFLAGS@ 
libilixi_types_la_CFLAGS 	= 	$(AM_CFLAGS)
libilixi_types_la_LIBADD 	= 	@DEPS_LIBS@
libilixi_types_la_SOURCES 	=	Affine2D.cpp \
								BoxLayout.cpp \
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
	          					WidgetLayout.cpp

ilixi_includedir 				= 	$(includedir)/$(PACKAGE)-$(VERSION)/types
nobase_ilixi_include_HEADERS 	= 	Affine2D.h \
									BoxLayout.h \
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
		          					WidgetLayout.h
