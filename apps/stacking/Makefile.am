## Makefile.am for apps/stacking
bin_PROGRAMS 			= 	ilixi_stacking
ilixi_stacking_LDADD	=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_stacking_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_stacking_CFLAGS	=	$(AM_CFLAGS)
ilixi_stacking_SOURCES	= 	Stacker.h \
							Stacker.cpp
