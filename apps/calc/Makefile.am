## Makefile.am for apps/calc
bin_PROGRAMS 			= 	ilixi_calc
ilixi_calc_LDADD		=	@DEPS_LIBS@ $(top_builddir)/$(PACKAGE)/lib$(PACKAGE)-$(VERSION).la $(AM_LDFLAGS)
ilixi_calc_CPPFLAGS		= 	-I$(top_srcdir)/$(PACKAGE) -I$(top_builddir)/$(PACKAGE) $(AM_CPPFLAGS) @DEPS_CFLAGS@
ilixi_calc_CFLAGS		=	$(AM_CFLAGS)
ilixi_calc_SOURCES		= 	Calc.h \
							Calc.cpp \
							Node.cpp \
							Node.h \
							NumNode.h \
							NumNode.cpp \
							OpNode.h \
							OpNode.cpp \
							Pair.cpp \
							Pair.h \
							Parser.cpp \
							Parser.h \
							UnaryNode.cpp \
							UnaryNode.h \
							Util.cpp \
							Util.h
