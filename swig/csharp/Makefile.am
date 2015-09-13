## Makefile.am for /swig/csharp

include ../common.mk
MAKEFLAGS 				:= -j 1
EXTRA_DIST 				= $(SWIG_SOURCES) Buttons.cs Dialogs.cs
BUILT_SOURCES 			= $(GEN_SOURCES) ilixi.dll
SWIG_INCLUDES			= @DEPS_CFLAGS@ -I$(top_srcdir)/ilixi
lib_LTLIBRARIES			= libilixi_cs.la
nodist_libilixi_cs_la_SOURCES = $(GEN_SOURCES)
libilixi_cs_la_SOURCES 	= $(SWIG_SOURCES)
libilixi_cs_la_CPPFLAGS = $(SWIG_INCLUDES)
libilixi_cs_la_LDFLAGS 	= -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE) -release $(LT_RELEASE) -L$(top_builddir)/$(PACKAGE)/ -module
libilixi_cs_la_LIBADD 	= -l$(PACKAGE)-$(VERSION)
SWIG_CSHARP_OPTIONS		= -namespace ilixi -small -outdir gen -dllimport "libilixi_cs.so" -c++ -csharp -module ilixi_$* -o

$(srcdir)/gen/ilixi_%.cpp : ../api/%.i
	mkdir -p gen
	$(SWIG) $(SWIG_INCLUDES) $(SWIG_CSHARP_OPTIONS) $@ $<
				
clean-local:
	-rm -rf gen *.dll *.exe
	
.PHONY	= csharp-tests

csharp-tests:
	$(CSC) -r:ilixi.dll Buttons.cs
	$(CSC) -r:ilixi.dll Dialogs.cs

ilixi.dll:
	$(CSC) -out:ilixi.dll -t:library gen/*.cs
		
install-exec-local: 
	cp ilixi.dll $(libdir)/ilixi.dll