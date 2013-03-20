## Makefile.am for apps/widgets
bin_PROGRAMS 			= 	ilixi_widgets
ilixi_widgets_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_widgets_CPPFLAGS 	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_widgets_CFLAGS	=	$(AM_CFLAGS)
ilixi_widgets_SOURCES	= 	WidgetsDemo.h \
							WidgetsDemo.cpp