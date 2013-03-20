## Makefile.am for apps/phone/home
bin_PROGRAMS 			= 	ilixi_phome
ilixi_phome_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_phome_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_phome_CFLAGS		=	$(AM_CFLAGS)
ilixi_phome_SOURCES		= 	AppButton.cpp \
    						AppButton.h \
    						Home.cpp \
    						Home.h
