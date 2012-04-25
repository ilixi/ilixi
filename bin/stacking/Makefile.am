## Makefile.am for bin/stacking
bin_PROGRAMS 			= 	ilixi_stacking
ilixi_stacking_LDADD	=	@DEPS_LIBS@ $(top_srcdir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_stacking_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_stacking_CFLAGS	=	$(AM_CFLAGS)
ilixi_stacking_SOURCES	= 	Stacker.h \
						Stacker.cpp
