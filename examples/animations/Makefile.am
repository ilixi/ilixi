## Makefile.am for examples/animations
bin_PROGRAMS				= 	ilixi_animations
ilixi_animations_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_animations_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_animations_CFLAGS		=	$(AM_CFLAGS)
ilixi_animations_SOURCES	= 	Spiral.h \
								Spiral.cpp \
								Animations.h \
								Animations.cpp
miscdatadir 	=	$(ILIXI_DATADIR)/images
miscdata_DATA 	=	circle.png
EXTRA_DIST 		= 	$(miscdata_DATA)