## Makefile.am for bin/gallery
SUBDIRS				=	images
bin_PROGRAMS 			= 	ilixi_demo1
ilixi_demo1_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_demo1_CPPFLAGS 	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_demo1_CFLAGS		=	$(AM_CFLAGS)
ilixi_demo1_SOURCES		= 	Gallery.h \
						Gallery.cpp \
						ImageDialog.h \
						ImageDialog.cpp \
						ImageWidget.h \
						ImageWidget.cpp
