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
PKG_PROG_PKG_CONFIG

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

AC_ARG_ENABLE([fusiondale], AS_HELP_STRING([--enable-fusiondale], [enable FusionDale support @<:@default=no@:>@]), [], [enable_fusiondale=no])
AC_ARG_ENABLE([fusionsound], AS_HELP_STRING([--enable-fusionsound], [enable FusionSound support @<:@default=no@:>@]), [], [enable_fusionsound=no])
AC_ARG_WITH([sawman], [AS_HELP_STRING([--with-sawman], [enables compositor and application manager])], [], [with_sawman=no])
AC_ARG_WITH([reflex], [AS_HELP_STRING([--with-reflex], [enable experimental support for reflex])], [], [with_reflex=no])
AC_ARG_WITH([examples], [AS_HELP_STRING([--with-examples], [compile examples])], [], [with_examples=no])

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

AC_CHECK_HEADERS_ONCE([stdlib.h stdarg.h stdio.h time.h unistd.h spawn.h])
AC_HEADER_STDBOOL
AC_TYPE_PID_T
AC_FUNC_ERROR_AT_LINE

# Checks for libraries.
DIRECTFB_REQ_VERSION=1.4.15
SAWMAN_REQ_VERSION=1.6.0
FUSIONDALE_REQ_VERSION=1.6.0
FUSIONSOUND_REQ_VERSION=1.6.0
SIGC_REQ_VERSION=2.2.4.2
LIBXML_REQ_VERSION=2.7.7
FONTCONFIG_REQ_VERSION=2.6.0

PACKAGES="directfb >= $DIRECTFB_REQ_VERSION sigc++-2.0 >= $SIGC_REQ_VERSION libxml-2.0 >= $LIBXML_REQ_VERSION fontconfig >= $FONTCONFIG_REQ_VERSION"

AC_MSG_NOTICE([checking dependencies...])
FUSIONDALE_DEP="fusiondale >= $FUSIONDALE_REQ_VERSION"
FUSIONSOUND_DEP="fusionsound >= $FUSIONSOUND_REQ_VERSION"
SAWMAN_DEP="sawman >= $SAWMAN_REQ_VERSION"
            
AS_IF([test "x$enable_fusiondale" = "xyes"], [
	PKG_CHECK_MODULES([FusionDale], [$FUSIONDALE_DEP], 
		[				
			PACKAGES="$PACKAGES $FUSIONDALE_DEP"
			AC_DEFINE([HAVE_FUSIONDALE], [1], [Enable FusionDale support])
		], 
		[
			enable_fusiondale=no 
			AC_MSG_WARN([*** FusionDale packages not found -- Building without FusionDale support.])
		]
	)], []
)

AS_IF([test "x$enable_fusionsound" = "xyes"], [
	PKG_CHECK_MODULES([FusionSound], [$FUSIONSOUND_DEP], 
		[	
			PACKAGES="$PACKAGES $FUSIONSOUND_DEP"
			AC_DEFINE([HAVE_FUSIONSOUND], [1], [Enable FusionSound support])
		], 
		[
			enable_fusionsound=no 
			AC_MSG_WARN([*** FusionSound packages not found -- Building without FusionSound support.])
		]
	)], []
)

AS_IF([test "x$with_sawman" = "xyes"], 
    [
        PKG_CHECK_MODULES([SaWMan], [$SAWMAN_DEP], 
        [
            PACKAGES="$PACKAGES $SAWMAN_DEP"
            AC_DEFINE([HAVE_SAWMAN], [1], [Enable SaWMan support])
        ],
        [
            with_sawman=no
            AC_MSG_WARN([*** SaWMan not found -- Building without SaWMan support.])
        ])
    ]
)

LIBREFLEX=""
AS_IF([test "x$with_reflex" = "xyes"], 
    [
        AC_CHECK_FILE([/opt/dfb/include/Reflex/Reflex.h], 
        [
            LIBREFLEX="-lReflex"
            AC_DEFINE([HAVE_LIBREFLEX], [1], [Enable Reflex support])
        ],
        [
            with_reflex=no
            AC_MSG_WARN([*** Reflex not found -- Building without Reflex support.])
        ])
    ]
)

PKG_CHECK_MODULES(DEPS, $PACKAGES)

AC_SUBST(DIRECTFB_REQ_VERSION)
AC_SUBST(SAWMAN_REQ_VERSION)
AC_SUBST(SIGC_REQ_VERSION)
AC_SUBST(LIBXML_REQ_VERSION)
AC_SUBST(FONTCONFIG_REQ_VERSION)
AC_SUBST(FUSIONDALE_DEP)
AC_SUBST(FUSIONSOUND_DEP)
AC_SUBST(SAWMAN_DEP)

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
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN([$PACKAGE_NAME], [doc/resources/doxygen.cfg], [doc])

######################################################################
# Output
######################################################################
DEPS_CFLAGS="$DEPS_CFLAGS -I/opt/dfb/include/"
DEPS_LIBS="$DEPS_LIBS $LIBREFLEX"
CXXFLAGS="$CFLAGS"

DFB_SURFACE_EVENTS=no
if $PKG_CONFIG --atleast-version=1.6.0 directfb; then
    DFB_SURFACE_EVENTS=yes
fi

AC_DEFINE_UNQUOTED([BINDIR], ["$prefix/bin/"], [ilixi binary directory])
AC_DEFINE_UNQUOTED([DATADIR], ["$prefix/share/$PACKAGE-$VERSION/"], [ilixi data directory])
AC_DEFINE_UNQUOTED([IMAGEDIR], ["$prefix/share/$PACKAGE-$VERSION/images/"], [ilixi image directory])

AM_CONDITIONAL([WITH_FUSIONDALE], [test x$enable_fusiondale = xyes])
AM_CONDITIONAL([WITH_FUSIONSOUND], [test x$enable_fusionsound = xyes])
AM_CONDITIONAL([WITH_SAWMAN], [test x$with_sawman = xyes])
AM_CONDITIONAL([WITH_REFLEX], [test x$with_reflex = xyes])
AM_CONDITIONAL([WITH_EXAMPLES], [test x$with_examples = xyes])
AM_CONDITIONAL([WITH_SURFACE_EVENTS], [test x$DFB_SURFACE_EVENTS = xyes])

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
		bin/meta-demo/Makefile \
		bin/monitor/Makefile \
		bin/navigation/Makefile \
		bin/osk/images/Makefile \
		bin/osk/Makefile \
		bin/settings/Makefile \
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
		doc/uml/Makefile \
		examples/Makefile \
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
   FusionDale support       : $enable_fusiondale
   FusionSound support      : $enable_fusionsound
   Sawman support           : $with_sawman
   Reflex support           : $with_reflex
   Examples                 : $with_examples

Type \`make' to build ilixi. 
Type \`make install' to install ilixi.
]);

echo $WITH_SURFACE_EVENTS