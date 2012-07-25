## Makefile.am for bin/soundmixer
bin_PROGRAMS 			= 	ilixi_mixer
ilixi_mixer_LDADD		=	@DEPS_LIBS@ $(top_srcdir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_mixer_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@ 
ilixi_mixer_CFLAGS		=	$(AM_CFLAGS)
ilixi_mixer_SOURCES		= 	SoundMixer.h \
							SoundMixer.cpp
