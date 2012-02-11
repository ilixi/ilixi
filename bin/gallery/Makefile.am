## Makefile.am for bin/gallery
SUBDIRS			=	images
bin_PROGRAMS 		= 	ilixi_demo1
ilixi_demo1_LDADD	=	$(top_srcdir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la  $(AM_LDFLAGS) @DEPS_LIBS@
ilixi_demo1_CPPFLAGS 	= 	-I$(top_srcdir)/$(PACKAGE) @DEPS_CFLAGS@
ilixi_demo1_SOURCES	= 	Gallery.h Gallery.cpp ImageDialog.h ImageDialog.cpp ImageWidget.h ImageWidget.cpp
