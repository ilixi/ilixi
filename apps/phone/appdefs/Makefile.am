## Makefile.am for apps/phone/appdefs
miscdatadir 			=	$(ILIXI_DATADIR)/phone/apps
miscdata_DATA 			=	ilixi_phome.appdef \
							ilixi_pstatus.appdef \
							osk.appdef
if WITH_BARESIP
miscdata_DATA 			+=	ilixi_sip.appdef
EXTRA_DIST 			= 	ilixi_dialer.appdef 
else
miscdata_DATA 			+=	ilixi_dialer.appdef
EXTRA_DIST 			= 	ilixi_sip.appdef 
endif
							
EXTRA_DIST 			+= 	$(miscdata_DATA) 
