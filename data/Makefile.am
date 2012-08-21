## Makefile.am for data
SUBDIRS			= 	apps fonts images
miscdatadir 	=	$(ILIXI_DATADIR)
miscdata_DATA 	=	def_palette.xml \
					def_style.xml \
					palette.dtd \
					style.dtd \
			ilixi_catalog.xml
			
EXTRA_DIST 		= 	$(miscdata_DATA)

install-data-hook:
	sed 's@DATADIR@$(ILIXI_DATADIR)/@' -i $(ILIXI_DATADIR)/ilixi_catalog.xml
