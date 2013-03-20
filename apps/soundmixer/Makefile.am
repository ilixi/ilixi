## Makefile.am for apps/soundmixer
SUBDIRS					=	images
bin_PROGRAMS 			= 	ilixi_mixer
ilixi_mixer_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_mixer_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@ 
ilixi_mixer_CFLAGS		=	$(AM_CFLAGS)
ilixi_mixer_SOURCES		= 	BandSlider.cpp \
							BandSlider.h \
							SoundMixer.h \
							SoundMixer.cpp \
							VolumeMeter.cpp \
							VolumeMeter.h						
