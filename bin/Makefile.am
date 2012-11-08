## Makefile.am for bin/
SUBDIRS = gallery demo2 demo3 demo4 carousel stacking navigation monitor
if WITH_FUSIONDALE

if WITH_SAWMAN
SUBDIRS += compositor home statusbar osk settings 
endif

if WITH_FUSIONSOUND
SUBDIRS += soundmixer
endif

endif # end WITH_FUSIONDALE

if WITH_REFLEX
SUBDIRS += meta-demo
endif