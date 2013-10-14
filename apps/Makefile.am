## Makefile.am for apps/
SUBDIRS = calc carousel demo2 demo4 gallery gestures monitor player stacking widgets

if WITH_FUSIONDALE

if WITH_BARESIP
SUBDIRS += sip
endif

if WITH_SAWMAN
SUBDIRS += osk settings car phone
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

#if WITH_REFLEX
#SUBDIRS += meta-demo
#endif
