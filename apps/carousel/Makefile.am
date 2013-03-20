## Makefile.am for apps/carousel
bin_PROGRAMS 			= 	ilixi_carousel
ilixi_carousel_LDADD	=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_carousel_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_carousel_CFLAGS	=	$(AM_CFLAGS)
ilixi_carousel_SOURCES	= 	CarouselDemo.h \
							CarouselDemo.cpp \
							../gallery/ImageWidget.h \
							../gallery/ImageWidget.cpp							
