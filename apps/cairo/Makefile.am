## Makefile.am for apps/cairo
bin_PROGRAMS 			= 	ilixi_cairo
ilixi_cairo_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_cairo_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_cairo_CFLAGS		=	$(AM_CFLAGS)
ilixi_cairo_SOURCES		= 	CairoDemo.h \
							CairoDemo.cpp \
							CairoClock.h \
							CairoClock.cpp
