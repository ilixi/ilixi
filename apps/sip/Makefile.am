## Makefile.am for apps/phone/sip
bin_PROGRAMS 			= 	ilixi_sip
ilixi_sip_LDADD			=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_sip_CPPFLAGS		= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_sip_CFLAGS		=	$(AM_CFLAGS)
ilixi_sip_SOURCES		= 	App.h \
							App.cpp \
							Call.cpp \
							Call.h \
							Contact.h \
							Contact.cpp \
							ContactManager.h \
							ContactManager.cpp \
							Contacts.cpp \
							Contacts.h \
							Dialer.h \
							Dialer.cpp \
							History.h \
							History.cpp \
							HistoryItem.h \
							HistoryItem.cpp \
							Settings.cpp \
							Settings.h \
							Status.cpp \
							Status.h
