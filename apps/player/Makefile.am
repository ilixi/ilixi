## Makefile.am for apps/player
bin_PROGRAMS 			= 	ilixi_player
ilixi_player_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_player_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_player_CFLAGS		=	$(AM_CFLAGS)
ilixi_player_SOURCES	= 	Player.h \
							Player.cpp							