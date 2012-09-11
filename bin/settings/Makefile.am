## Makefile.am for bin/settings
bin_PROGRAMS 				= 	ilixi_settings
ilixi_settings_LDADD		=	@DEPS_LIBS@ $(top_srcdir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_settings_CPPFLAGS 	= 	-I$(top_srcdir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_settings_CFLAGS		=	$(AM_CFLAGS)
ilixi_settings_SOURCES		= 	Settings.h \
								Settings.cpp