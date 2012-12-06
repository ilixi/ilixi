## Makefile.am for examples
SUBDIRS					= animations
bin_PROGRAMS 			= ilixi_hello ilixi_buttons ilixi_containers ilixi_signals ilixi_dialogs ilixi_animatedtiles

ILIXI_EX_LDLIBS			= @DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ILIXI_EX_CPPFLAGS		= -I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ILIXI_EX_CFLAGS			= $(AM_CFLAGS)

ilixi_hello_LDADD		= $(ILIXI_EX_LDLIBS)
ilixi_hello_CPPFLAGS	= $(ILIXI_EX_CPPFLAGS)
ilixi_hello_CFLAGS		= $(ILIXI_EX_CFLAGS)
ilixi_hello_SOURCES		= hello_world.cpp

ilixi_buttons_LDADD		= $(ILIXI_EX_LDLIBS)
ilixi_buttons_CPPFLAGS	= $(ILIXI_EX_CPPFLAGS)
ilixi_buttons_CFLAGS	= $(ILIXI_EX_CFLAGS)
ilixi_buttons_SOURCES	= buttons.cpp

ilixi_containers_LDADD		= $(ILIXI_EX_LDLIBS)
ilixi_containers_CPPFLAGS	= $(ILIXI_EX_CPPFLAGS)
ilixi_containers_CFLAGS		= $(ILIXI_EX_CFLAGS)
ilixi_containers_SOURCES	= containers.cpp

ilixi_signals_LDADD		= $(ILIXI_EX_LDLIBS)
ilixi_signals_CPPFLAGS	= $(ILIXI_EX_CPPFLAGS)
ilixi_signals_CFLAGS	= $(ILIXI_EX_CFLAGS)
ilixi_signals_SOURCES	= signals.cpp

ilixi_dialogs_LDADD		= $(ILIXI_EX_LDLIBS)
ilixi_dialogs_CPPFLAGS	= $(ILIXI_EX_CPPFLAGS)
ilixi_dialogs_CFLAGS	= $(ILIXI_EX_CFLAGS)
ilixi_dialogs_SOURCES	= dialogs.cpp

ilixi_animatedtiles_LDADD	= $(ILIXI_EX_LDLIBS)
ilixi_animatedtiles_CPPFLAGS	= $(ILIXI_EX_CPPFLAGS)
ilixi_animatedtiles_CFLAGS	= $(ILIXI_EX_CFLAGS)
ilixi_animatedtiles_SOURCES	= animatedtiles.cpp

# .. in progress ..
#if WITH_REFLEX
#SUBDIRS += meta-ui
#endif