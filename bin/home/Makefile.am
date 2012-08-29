## Makefile.am for bin/home
bin_PROGRAMS 		= 	ilixi_home
ilixi_home_LDADD	=	@DEPS_LIBS@ $(top_srcdir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_home_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_home_CFLAGS	=	$(AM_CFLAGS)
ilixi_home_SOURCES	= 	AppButton.cpp \
						AppButton.h \
						Home.cpp \
						Home.h
