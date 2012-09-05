## Makefile.am for bin/demo3
bin_PROGRAMS 			= 	ilixi_demo3
ilixi_demo3_LDADD		=	@DEPS_LIBS@ $(top_srcdir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_demo3_CPPFLAGS 	= 	-I$(top_srcdir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_demo3_CFLAGS		=	$(AM_CFLAGS)
ilixi_demo3_SOURCES		= 	Demo3.h \
							Demo3.cpp