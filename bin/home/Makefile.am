## Makefile.am for bin/home
bin_PROGRAMS 		= 	ilixi_home
ilixi_home_LDADD	=	@DEPS_LIBS@ $(top_srcdir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la  $(AM_LDFLAGS)
ilixi_home_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) @DEPS_CFLAGS@
ilixi_home_SOURCES	= 	Home.cpp Home.h
