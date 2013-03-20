## Makefile.am for apps/home
bin_PROGRAMS 			= 	ilixi_carhome
ilixi_carhome_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_carhome_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_carhome_CFLAGS	=	$(AM_CFLAGS)
ilixi_carhome_SOURCES	= 	AppButton.cpp \
    						AppButton.h \
    						Home.cpp \
    						Home.h \
    						NumPages.cpp \
    						NumPages.h \
    						PageButton.cpp \
    						PageButton.h

miscdatadir 			=	$(ILIXI_DATADIR)/car/home
miscdata_DATA 			=	circle.png circle_small.png
EXTRA_DIST 				= 	$(miscdata_DATA)
