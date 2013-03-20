## Makefile.am for apps/gallery
SUBDIRS					=	images
bin_PROGRAMS 			= 	ilixi_gallery
ilixi_gallery_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_gallery_CPPFLAGS 	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_gallery_CFLAGS	=	$(AM_CFLAGS)
ilixi_gallery_SOURCES	= 	Gallery.h \
    						Gallery.cpp \
    						ImageDialog.h \
    						ImageDialog.cpp \
    						ImageWidget.h \
    						ImageWidget.cpp
