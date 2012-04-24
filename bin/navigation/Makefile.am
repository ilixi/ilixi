## Makefile.am for bin/navigation
bin_PROGRAMS 		= 	ilixi_nav
ilixi_nav_LDADD	=	@DEPS_LIBS@ $(top_srcdir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la  $(AM_LDFLAGS)
ilixi_nav_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) @DEPS_CFLAGS@ $(AM_CFLAGS)
ilixi_nav_SOURCES	= 	Navigation.h Navigation.cpp
