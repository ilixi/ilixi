## Makefile.am for apps/car/compositor
SUBDIRS					=	images
bin_PROGRAMS 			= 	ilixi_carcomp
ilixi_carcomp_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_carcomp_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_carcomp_CFLAGS	=	$(AM_CFLAGS)
ilixi_carcomp_SOURCES	= 	CarCompositor.cpp \
							CarCompositor.h \
							HorizontalScroller.cpp \
							HorizontalScroller.h \
							HorizontalSwitcher.cpp \
							HorizontalSwitcher.h
