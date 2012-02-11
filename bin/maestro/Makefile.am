## Makefile.am for bin/maestro
miscdatadir 		=	$(ILIXI_DATADIR)
miscdata_DATA 		=	maestro.conf
EXTRA_DIST 		= 	$(miscdata_DATA)
bin_PROGRAMS 		= 	ilixi_maestro
ilixi_maestro_LDADD	=	$(top_srcdir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la  $(AM_LDFLAGS) @DEPS_LIBS@
ilixi_maestro_CPPFLAGS 	= 	-I$(top_srcdir)/$(PACKAGE) @DEPS_CFLAGS@
ilixi_maestro_SOURCES	=  	Maestro.cpp \
				AppManager.h \
				AppManager.cpp \
				Layout.h \
				Layout.cpp 
