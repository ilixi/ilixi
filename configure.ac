######################################################################
# Process this file with autoconf to produce a configure script.
######################################################################
AC_INIT([ilixi],[0.1],[tarik@ilixi.org],[ilixi],[http://www.ilixi.org])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([config/m4])
AC_CONFIG_HEADERS([config.h])
AX_PREFIX_CONFIG_H(ilixi/ilixiConfig.h)
AC_PREREQ(2.65)

######################################################################
# Versioning
# Each release increases ILIXI_MICRO_VERSION, ILIXI_INTERFACE_AGE and ILIXI_BINARY_AGE by 1.
# if any functions have been added, set ILIXI_INTERFACE_AGE to 0.
# If backwards compatibility has been broken, set ILIXI_INTERFACE_AGE and ILIXI_BINARY_AGE to 0.
######################################################################
ILIXI_MAJOR_VERSION=0
ILIXI_MINOR_VERSION=1
ILIXI_MICRO_VERSION=0
ILIXI_INTERFACE_AGE=0
ILIXI_BINARY_AGE=0
ILIXI_VERSION=$ILIXI_MAJOR_VERSION.$ILIXI_MINOR_VERSION.$ILIXI_MICRO_VERSION

AC_SUBST(ILIXI_MAJOR_VERSION)
AC_SUBST(ILIXI_MINOR_VERSION)
AC_SUBST(ILIXI_MICRO_VERSION)
AC_SUBST(ILIXI_INTERFACE_AGE)
AC_SUBST(ILIXI_BINARY_AGE)
AC_SUBST(ILIXI_VERSION)

# libtool versioning
LT_RELEASE=$ILIXI_MAJOR_VERSION.$ILIXI_MINOR_VERSION
LT_CURRENT=`expr $ILIXI_MICRO_VERSION - $ILIXI_INTERFACE_AGE`
LT_BINARY=`expr $ILIXI_MICRO_VERSION - $ILIXI_BINARY_AGE`
LT_REVISION=$ILIXI_INTERFACE_AGE
LT_AGE=`expr $ILIXI_BINARY_AGE - $ILIXI_INTERFACE_AGE`

AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_BINARY)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

# For automake
PACKAGE=ilixi
VERSION=$ILIXI_VERSION
AM_INIT_AUTOMAKE($PACKAGE, $VERSION, no-define)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

######################################################################
# Options
######################################################################
LOGGER_LEVEL="none"
AC_ARG_ENABLE([log], AS_HELP_STRING([--enable-log], [enables logger and submits up to error level messages @<:@default=no@:>@]), [LOGGER_LEVEL="up to error"])
if test "$enable_log" = "yes"; then
	AC_DEFINE([LOGGER_ENABLED], [], [Enable logger])
fi

AC_ARG_ENABLE([log-debug], AS_HELP_STRING([--enable-log-debug], [enables logger and submits up to debug level messages @<:@default=no@:>@]), [LOGGER_LEVEL="up to debug"])
if test "$enable_log_debug" = "yes"; then
	AC_DEFINE([LOGGER_ENABLED], [], [Enable logger])
	AC_DEFINE([LOG_DEBUG_ENABLED], [], [Log debug messages])
	CFLAGS="$CFLAGS -DDIRECT_ENABLE_DEBUG"
fi

AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug], [enable debugging and trace support @<:@default=no@:>@]), [], [enable_debug=no])
if test "$enable_debug" = "yes"; then
    CFLAGS="$CFLAGS -g3 -O0 -fno-inline -Wno-inline -finstrument-functions"
	CPPFLAGS="$CPPFLAGS -DDIRECT_FORCE_DEBUG"
fi

AC_ARG_ENABLE([profiling], AS_HELP_STRING([--enable-profiling], [enable profiling @<:@default=no@:>@]), [], [enable_profiling=no])
if test "$enable_profiling" = "yes"; then
    CFLAGS="$CFLAGS -pg"
fi

AC_ARG_ENABLE([stereo], AS_HELP_STRING([--enable-stereo], [enable stereo @<:@default=no@:>@]), [], [enable_stereo=no])
if test "$enable_stereo" = "yes"; then
	AC_DEFINE([STEREO_OUTPUT], [], [Enable side by side stereo output])
fi

AC_ARG_ENABLE([dale], AS_HELP_STRING([--enable-dale], [enable FusionDale support @<:@default=no@:>@]), [], [enable_dale=no])
AM_CONDITIONAL([WITH_FUSIONDALE], [test x$enable_dale = xyes])

AC_ARG_ENABLE([sound], AS_HELP_STRING([--enable-sound], [enable FusionSound support @<:@default=no@:>@]), [], [enable_sound=no])
AM_CONDITIONAL([WITH_FUSIONSOUND], [test x$enable_sound = xyes])

######################################################################
# Dependencies
######################################################################
# Checks for programs.
LT_INIT
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_CHECK_HEADERS([stdlib.h stdarg.h stdio.h time.h unistd.h spawn.h])
AC_HEADER_STDBOOL
AC_TYPE_PID_T
AC_FUNC_ERROR_AT_LINE

# Checks for libraries.
DIRECTFB_REQ_VERSION=1.6.0
SAWMAN_REQ_VERSION=1.6.0
FUSIONDALE_REQ_VERSION=1.6.0
FUSIONSOUND_REQ_VERSION=1.6.0
SIGC_REQ_VERSION=2.2.4.2
LIBXML_REQ_VERSION=2.7.7
FONTCONFIG_REQ_VERSION=2.8.0

PACKAGES="directfb >= $DIRECTFB_REQ_VERSION sawman >= $SAWMAN_REQ_VERSION sigc++-2.0 >= $SIGC_REQ_VERSION libxml-2.0 >= $LIBXML_REQ_VERSION fontconfig >= FONTCONFIG_REQ_VERSION"

AC_MSG_NOTICE([checking dependencies...])
AS_IF([test "x$enable_dale" = "xyes"], [
	FUSION_DALE_DEP="fusiondale >= $FUSIONDALE_REQ_VERSION"
	PACKAGES="$PACKAGES $FUSION_DALE_DEP"
	AC_DEFINE([HAVE_FUSIONDALE], [1], [Enable FusionDale support])
], [

])

AS_IF([test "x$enable_sound" = "xyes"], [
	FUSION_SOUND_DEP="fusionsound >= $FUSIONSOUND_REQ_VERSION"
	PACKAGES="$PACKAGES $FUSION_SOUND_DEP"
	AC_DEFINE([HAVE_FUSIONSOUND], [1], [Enable FusionSound support])
], [

])

PKG_CHECK_MODULES(DEPS, $PACKAGES)

AC_SUBST(DIRECTFB_REQ_VERSION)
AC_SUBST(SAWMAN_REQ_VERSION)
AC_SUBST(SIGC_REQ_VERSION)
AC_SUBST(LIBXML_REQ_VERSION)
AC_SUBST(FONTCONFIG_REQ_VERSION)
AC_SUBST(FUSION_DALE_DEP)
AC_SUBST(FUSION_SOUND_DEP)

######################################################################
# DOXYGEN SUPPORT
######################################################################
DX_DOXYGEN_FEATURE(ON)
DX_DOT_FEATURE(ON)
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(ON)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN([$PACKAGE_NAME], [doc/resources/doxygen.cfg], [doc])

######################################################################
# Output
######################################################################
CXXFLAGS="$CFLAGS"
AC_DEFINE_UNQUOTED([BINDIR], ["$prefix/bin/"], [ilixi binary directory])
AC_DEFINE_UNQUOTED([DATADIR], ["$prefix/share/$PACKAGE-$VERSION/"], [ilixi data directory])
AC_DEFINE_UNQUOTED([IMAGEDIR], ["$prefix/share/$PACKAGE-$VERSION/images/"], [ilixi image directory])

ILIXI_DATADIR=$prefix/share/$PACKAGE-$VERSION
AC_SUBST(ILIXI_DATADIR)

AC_CONFIG_FILES([ \
		Makefile  \
		bin/Makefile \
		bin/carousel/Makefile \
		bin/compositor/images/Makefile \
		bin/compositor/Makefile \
		bin/demo2/Makefile \
		bin/demo3/Makefile \
		bin/demo4/Makefile \
		bin/gallery/images/Makefile \
		bin/gallery/Makefile \
		bin/home/Makefile \
		bin/navigation/Makefile \
		bin/osk/images/Makefile \
		bin/osk/Makefile \
		bin/soundmixer/images/Makefile \
		bin/soundmixer/Makefile \
		bin/stacking/Makefile \
		bin/statusbar/images/Makefile \
		bin/statusbar/Makefile \
		data/Makefile \
		data/apps/icons/Makefile \
		data/apps/Makefile \
		data/ilixi_catalog.xml \
		data/images/Makefile \
		doc/Makefile \
		doc/resources/Makefile \
		ilixi/Makefile \
		ilixi/core/Makefile \
		ilixi/graphics/Makefile \
		ilixi/lib/Makefile \
		ilixi/types/Makefile \
		ilixi/ui/Makefile \
		ilixi.pc:ilixi.pc.in
])
AC_OUTPUT

AC_MSG_RESULT([
   ilixi configuration summary
  -----------------------------
   Version                  : $VERSION
   Install prefix           : $prefix
   Build shared libs        : $enable_shared
   Build static libs        : $enable_static
   CPPFLAGS                 : $CPPFLAGS
   CFLAGS                   : $CFLAGS
   LDFLAGS                  : $LDFLAGS
   DEPS_CFLAGS              : $DEPS_CFLAGS
   DEPS_LIBS                : $DEPS_LIBS
   Log level                : $LOGGER_LEVEL
   Stereoscopy support      : $enable_stereo   
   FusionDale support       : $enable_dale
   FusionSound support      : $enable_sound

Type \`make' to build ilixi. 
Type \`make install' to install ilixi.
]);

AS_IF([test "x$enable_dale" = "xno"], [
	AC_MSG_WARN([ilixi compositor requires FusionDale support; so it won't build.])
])
