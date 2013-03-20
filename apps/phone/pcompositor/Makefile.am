## Makefile.am for apps/phone/compositor
bin_PROGRAMS 			= 	ilixi_pcomp
ilixi_pcomp_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_pcomp_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_pcomp_CFLAGS		=	$(AM_CFLAGS)
ilixi_pcomp_SOURCES		= 	PhoneCompositor.cpp \
							PhoneCompositor.h \
							CarouselSwitcher.cpp \
							CarouselSwitcher.h
