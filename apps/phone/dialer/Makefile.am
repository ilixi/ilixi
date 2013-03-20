## Makefile.am for apps/phone/dialer
bin_PROGRAMS 			= 	ilixi_dialer
ilixi_dialer_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_dialer_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_dialer_CFLAGS		=	$(AM_CFLAGS)
ilixi_dialer_SOURCES	= 	Dialer.cpp \
							Dialer.h
