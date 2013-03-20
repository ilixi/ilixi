## Makefile.am for apps/phone/statusbar
SUBDIRS					= 	images
bin_PROGRAMS 			= 	ilixi_pstatus
ilixi_pstatus_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_pstatus_CPPFLAGS	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) -I$(top_srcdir)/apps/monitor $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_pstatus_CFLAGS	=	$(AM_CFLAGS)
ilixi_pstatus_SOURCES	= 	Clock.h \
							Clock.cpp \
							StatusBar.cpp \
							StatusBar.h \
							NotificationIcon.h \
							NotificationIcon.cpp \
							ListItem.h \
							ListItem.cpp \
							$(top_srcdir)/apps/monitor/BarChart.h \
							$(top_srcdir)/apps/monitor/BarChart.cpp \
							$(top_srcdir)/apps/monitor/CPUMonitor.h \
							$(top_srcdir)/apps/monitor/CPUMonitor.cpp