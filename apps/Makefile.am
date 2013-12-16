## Makefile.am for apps/
SUBDIRS =
if WITH_DEMOS
SUBDIRS += calc carousel demo2 demo4 gallery gestures monitor player stacking widgets
if WITH_FUSIONDALE

if WITH_BARESIP
SUBDIRS += sip
endif

if WITH_FUSIONSOUND
SUBDIRS += soundmixer
endif

endif # end WITH_FUSIONDALE

if WITH_EGL
SUBDIRS += egl
endif

if WITH_CAIRO
SUBDIRS += cairo
endif
endif # end WITH_DEMOS

if WITH_COMPOSITOR
SUBDIRS += osk
if WITH_DEMOS
SUBDIRS += settings car phone
endif
endif # end WITH_COMPOSITOR

#if WITH_REFLEX
#SUBDIRS += meta-demo
#endif
