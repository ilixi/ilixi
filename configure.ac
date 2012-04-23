######################################################################
# Process this file with autoconf to produce a configure script.
######################################################################
AC_INIT([ilixi],[0.1],[tarik@ilixi.org],[ilixi],[http://www.ilixi.org])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([config/m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([ilixi/ilixiGUI.h])
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
# Conditional inclusions
######################################################################
ILIXI_LOGGER_ENABLED=0
LOGGER_LEVEL="none"
AC_SUBST(ILIXI_LOGGER_ENABLED)
AC_ARG_ENABLE([log], AS_HELP_STRING([--enable-log], [enables logger and submits up to error level messages @<:@default=no@:>@]), [ILIXI_LOGGER_ENABLED=1 LOGGER_LEVEL="up to error"])

ILIXI_LOG_DEBUG_ENABLED=0
AC_SUBST(ILIXI_LOG_DEBUG_ENABLED)
AC_ARG_ENABLE([log-debug], AS_HELP_STRING([--enable-log-debug], [enables logger and submits up to debug level messages @<:@default=no@:>@]), [ILIXI_LOGGER_ENABLED=1 ILIXI_LOG_DEBUG_ENABLED=1 LOGGER_LEVEL="up to debug" CPPFLAGS="-DDIRECT_ENABLE_DEBUG $CPPFLAGS"])

AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug], [enable debugging @<:@default=no@:>@]), [], [enable_debug=no])
if test "$enable_debug" = "yes"; then
    CPPFLAGS="-g3 -O0 -fno-inline -Wno-inline $CPPFLAGS"
    ILIXI_BUILD_DEBUG=1
else
    ILIXI_BUILD_DEBUG=0
fi
AC_SUBST(ILIXI_BUILD_DEBUG)

AM_CONDITIONAL(ENABLE_DEBUG, test "$enable_debug" = "yes")

######################################################################
# Dependencies
######################################################################
# Checks for programs.
LT_INIT
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_HEADER_SYS_WAIT

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h stdarg.h stdio.h time.h unistd.h spawn.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_PID_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE

# Checks for libraries.
DIRECTFB_REQ_VERSION=1.4.9
SIGC_REQ_VERSION=2.2.4.2
LIBXML_REQ_VERSION=2.7.7

AC_MSG_NOTICE([checking if dependencies are installed...])
PKG_CHECK_MODULES(DEPS, directfb >= $DIRECTFB_REQ_VERSION sigc++-2.0 >= $SIGC_REQ_VERSION libxml-2.0 >= $LIBXML_REQ_VERSION)

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
ILIXI_DATADIR=$datadir/$PACKAGE-$VERSION
AC_SUBST(ILIXI_DATADIR)

AC_CONFIG_FILES([ \
		Makefile  \
		data/Makefile \
		data/fonts/Makefile \
		data/images/Makefile \
		doc/Makefile \
		doc/resources/Makefile \
		ilixi/Makefile \
		ilixi/ilixiConfig.h \
		ilixi/core/Makefile \
		ilixi/graphics/Makefile \
		ilixi/lib/Makefile \
		ilixi/types/Makefile \
		ilixi/ui/Makefile \
		bin/Makefile \
		bin/maestro/Makefile \
		bin/statusbar/Makefile \
		bin/statusbar/images/Makefile \
		bin/home/Makefile \
		bin/gallery/Makefile \
		bin/gallery/images/Makefile \
		bin/demo2/Makefile \
		bin/demo3/Makefile \
		bin/demo4/Makefile \
		bin/stacking/Makefile \
		bin/compositor/Makefile \
		bin/compositor/images/Makefile \
		ilixi-$VERSION.pc:ilixi.pc.in
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
   Log level                : $LOGGER_LEVEL   

Type \`make' to build ilixi. 
Type \`make install' to install ilixi.
]);
