## Makefile.am for apps/demo4
bin_PROGRAMS 			= 	ilixi_demo4
ilixi_demo4_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_demo4_CPPFLAGS 	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_demo4_CFLAGS		=	$(AM_CFLAGS)
ilixi_demo4_SOURCES		= 	Demo4.h \
                              Demo4.cpp \
                              VKMenu.h \
                              VKMenu.cpp \
                              MenuItem.h \
                              MenuItem.cpp \
                              ImageDialog.h \
                              ImageDialog.cpp