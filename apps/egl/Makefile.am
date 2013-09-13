## Makefile.am for apps/egl
bin_PROGRAMS 		= 	ilixi_egl
ilixi_egl_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_egl_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_egl_CFLAGS	=	$(AM_CFLAGS)
ilixi_egl_SOURCES	= 	es2tri.h \
						GLTri.h \
						GLTri.cpp \
						GLDemo.h \
						GLDemo.cpp
