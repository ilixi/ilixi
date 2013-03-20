## Makefile.am for apps/monitor
bin_PROGRAMS 			= 	ilixi_monitor
ilixi_monitor_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la  $(AM_LDFLAGS)
ilixi_monitor_CFLAGS	=	$(AM_CFLAGS)
ilixi_monitor_CPPFLAGS 	= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_monitor_SOURCES	= 	CPUMonitor.cpp \
							CPUMonitor.h \
							FSMonitor.cpp \
							FSMonitor.h \
							MEMMonitor.cpp \
							MEMMonitor.h \
							Monitor.cpp \
							Monitor.h \
							NETMonitor.cpp \
							NETMonitor.h \
							OSMonitor.cpp \
							OSMonitor.h \
							BarChart.h \
							BarChart.cpp