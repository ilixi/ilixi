## Makefile.am for apps/demo2
bin_PROGRAMS 			= 	ilixi_demo2
ilixi_demo2_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_demo2_CPPFLAGS 	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_demo2_CFLAGS		=	$(AM_CFLAGS)
ilixi_demo2_SOURCES		= 	Demo2.h \
							Demo2.cpp \
							ScrollItem.h \
							ScrollItem.cpp
