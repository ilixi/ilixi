$(builddir)/%_rflx.cpp: $(srcdir)/%.h
	GCCXML_CXXFLAGS="$(INCLUDES) $(DEPS_CFLAGS)" genreflex $< --no_membertypedefs --quiet
