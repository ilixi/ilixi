## Makefile.am for data
SUBDIRS			= 	images sounds themes

if WITH_COMPOSITOR
SUBDIRS			+=	apps
endif

miscdatadir 	=	$(ILIXI_DATADIR)
miscdata_DATA 	=	application.dtd \
					compositor.dtd \
					compositor.xml \
					configuration.dtd \
					font_pack.dtd \
					icon_pack.dtd \
					image_pack.dtd \
					keyboard.dtd \
					palette.dtd \
					style.dtd \
					ilixi_catalog.xml \
					ilixi_config.xml
			
EXTRA_DIST 		= 	$(miscdata_DATA)
