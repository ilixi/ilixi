## top directory
include $(top_srcdir)/doc/aminclude.am
dist_noinst_SCRIPTS	= 	autogen.sh
ilixi_libincludedir	= 	$(libdir)/$(PACKAGE)-$(VERSION)/include
pkgconfigdir 		= 	$(libdir)/pkgconfig
pkgconfig_DATA 		= 	$(PACKAGE).pc
ACLOCAL_AMFLAGS 	= 	${ACLOCAL_FLAGS} -I config/m4
AUTOMAKE_OPTIONS	= 	subdir-objects
SUBDIRS				= 	$(PACKAGE) data apps 
if WITH_EXAMPLES
SUBDIRS				+=	examples
endif
DIST_SUBDIRS		= 	$(PACKAGE) data apps doc examples
EXTRA_DIST 			= 	COPYING \
						COPYING.LESSER \
						README \
						TODO \
						ChangeLog \
						config/m4
MOSTLYCLEANFILES 	= 	$(DX_CLEANFILES)
