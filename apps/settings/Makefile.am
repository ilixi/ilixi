## Makefile.am for apps/settings
bin_PROGRAMS 				= 	ilixi_settings
ilixi_settings_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_settings_CPPFLAGS 	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_settings_CFLAGS		=	$(AM_CFLAGS)
ilixi_settings_SOURCES		= 	Settings.h \
								Settings.cpp