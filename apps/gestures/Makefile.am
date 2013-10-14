## Makefile.am for apps/gestures
bin_PROGRAMS 			= 	ilixi_gestures
ilixi_gestures_LDADD	=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_gestures_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_gestures_CFLAGS	=	$(AM_CFLAGS)
ilixi_gestures_SOURCES	= 	GestureDemo.h \
							GestureDemo.cpp
							
miscdatadir 			=	$(ILIXI_DATADIR)/gestures
miscdata_DATA 			=	gestures.png
EXTRA_DIST 				= 	$(miscdata_DATA)							