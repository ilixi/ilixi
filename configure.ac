######################################################################
# Process this file with autoconf to produce a configure script.
######################################################################
AC_INIT([ilixi],[1.0.0],[tarik@ilixi.org],[ilixi],[http://www.ilixi.org])
AC_CONFIG_SRCDIR([ilixi_config.h.in])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([config/m4])
AC_CONFIG_HEADERS([ilixi_config.h])
AX_PREFIX_CONFIG_H(ilixi/ilixiConfig.h)
AC_PREREQ(2.65)

######################################################################
# Versioning
# Each release increases ILIXI_MICRO_VERSION, ILIXI_INTERFACE_AGE and ILIXI_BINARY_AGE by 1.
# if any functions have been added, set ILIXI_INTERFACE_AGE to 0.
# If backwards compatibility has been broken, set ILIXI_INTERFACE_AGE and ILIXI_BINARY_AGE to 0.
######################################################################
ILIXI_MAJOR_VERSION=1
ILIXI_MINOR_VERSION=0
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
AC_ARG_ENABLE([log], AS_HELP_STRING([--enable-log], [enables logger and submits up to error level messages @<:@default=yes@:>@]), [LOGGER_LEVEL="up to error"])
if test "$enable_log" = "yes"; then
	AC_DEFINE([LOGGER_ENABLED], [], [Enable logger])
fi

AC_ARG_ENABLE([log-debug], AS_HELP_STRING([--enable-log-debug], [enables logger and submits up to debug level messages @<:@default=no@:>@]), [LOGGER_LEVEL="up to debug"])
if test "$enable_log_debug" = "yes"; then
	AC_DEFINE([LOGGER_ENABLED], [], [Enable logger])
	AC_DEFINE([LOG_DEBUG_ENABLED], [], [Log debug messages])
	CFLAGS="$CFLAGS -DDIRECT_ENABLE_DEBUG"
fi

AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug], [enable debugging @<:@default=no@:>@]), [], [enable_debug=no])
if test "$enable_debug" = "yes"; then
    CFLAGS="$CFLAGS -g3 -O0 -fno-inline -Wno-inline"
	CPPFLAGS="$CPPFLAGS -DDIRECT_FORCE_DEBUG"
fi

AC_ARG_ENABLE([trace], AS_HELP_STRING([--enable-trace], [enable trace support @<:@default=no@:>@]), [], [enable_trace=no])
if test "$enable_trace" = "yes"; then
    CFLAGS="$CFLAGS -finstrument-functions"
fi

AC_ARG_ENABLE([profiling], AS_HELP_STRING([--enable-profiling], [enable profiling @<:@default=no@:>@]), [], [enable_profiling=no])
if test "$enable_profiling" = "yes"; then
    CFLAGS="$CFLAGS -pg"
fi

AC_ARG_ENABLE([stereoscopy], AS_HELP_STRING([--enable-stereoscopy], [enable stereoscopy support @<:@default=no@:>@]), [], [enable_stereoscopy=no])
if test "$enable_stereoscopy" = "yes"; then
	AC_DEFINE([STEREO_OUTPUT], [], [Enable side by side stereoscopy output])
fi

AC_ARG_ENABLE([compositor], [AS_HELP_STRING([--enable-compositor], [enable compositor support @<:@default=no@:>@])], [enable_fusiondale=yes enable_sawman=yes], [enable_compositor=no])
AC_ARG_ENABLE([egl], [AS_HELP_STRING([--enable-egl], [enable DFBEGL support @<:@default=no@:>@])], [], [enable_egl=no])
AC_ARG_ENABLE([cairo], [AS_HELP_STRING([--enable-cairo], [enable cairo support @<:@default=no@:>@])], [], [enable_cairo=no])
AC_ARG_ENABLE([fusiondale], AS_HELP_STRING([--enable-fusiondale], [enable FusionDale support @<:@default=no@:>@]), [], [enable_fusiondale=no])
AC_ARG_ENABLE([fusionsound], AS_HELP_STRING([--enable-fusionsound], [enable FusionSound support @<:@default=no@:>@]), [], [enable_fusionsound=no])
AC_ARG_ENABLE([sawman], [AS_HELP_STRING([--enable-sawman], [enable compositor and application manager features @<:@default=no@:>@])], [], [enable_sawman=no])
AC_ARG_ENABLE([reflex], [AS_HELP_STRING([--enable-reflex], [enable experimental support for Reflex @<:@default=no@:>@])], [], [enable_reflex=no])
AC_ARG_ENABLE([wnn], [AS_HELP_STRING([--enable-wnn], [enable experimental support for Wnn @<:@default=no@:>@])], [], [enable_wnn=no])
AC_ARG_ENABLE([nls], [AS_HELP_STRING([--enable-nls], [enable Native Language Support via message catalogs @<:@default=no@:>@])], [], [enable_nls=no])
AC_ARG_ENABLE([wstring], [AS_HELP_STRING([--enable-wstring], [enable wstrings in text layouts @<:@default=no@:>@])], [], [enable_wstring=no])
AC_ARG_WITH([demos], [AS_HELP_STRING([--with-demos], [build demo applications @<:@default=yes@:>@])], [], [with_demos=yes])
AC_ARG_WITH([examples], [AS_HELP_STRING([--with-examples], [compile examples @<:@default=no@:>@])], [], [with_examples=no])

AC_ARG_WITH([focus-overlays], [AC_HELP_STRING([--with-focus-overlays], [enables drawing of focus overlay images) @<:@default=yes@:>@])], [], [with_focus_overlays=yes])
AC_ARG_WITH([csharp], [AC_HELP_STRING([--with-csharp], [enable CSharp bindings @<:@default=no@:>@])], [], [with_csharp=no])

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
AX_PKG_SWIG(1.3.21)

AC_CHECK_HEADERS_ONCE([stdlib.h stdarg.h stdio.h time.h unistd.h spawn.h])
AC_HEADER_STDBOOL
AC_TYPE_PID_T
AC_FUNC_ERROR_AT_LINE

# Checks for libraries.
DIRECTFB_REQ_VERSION=1.6.3
SAWMAN_REQ_VERSION=1.6.0
FUSIONDALE_REQ_VERSION=0.8.2
FUSIONSOUND_REQ_VERSION=1.6.0
SIGC_REQ_VERSION=2.2.4.2
LIBXML_REQ_VERSION=2.7.7
FONTCONFIG_REQ_VERSION=2.6.0
DFBEGL_REQ_VERSION=7.10
CAIRO_REQ_VERSION=1.10.2

REQ_PACKAGES="directfb >= $DIRECTFB_REQ_VERSION sigc++-2.0 >= $SIGC_REQ_VERSION libxml-2.0 >= $LIBXML_REQ_VERSION fontconfig >= $FONTCONFIG_REQ_VERSION"

AC_MSG_NOTICE([checking dependencies...])
EGL_DEP="egl >= $DFBEGL_REQ_VERSION"
CAIRO_DEP="cairo-directfb >= $CAIRO_REQ_VERSION"
FUSIONDALE_DEP="fusiondale >= $FUSIONDALE_REQ_VERSION"
FUSIONSOUND_DEP="fusionsound >= $FUSIONSOUND_REQ_VERSION"
SAWMAN_DEP="sawman >= $SAWMAN_REQ_VERSION"

AS_IF([test "x$enable_egl" = "xyes"], 
    [
    	PKG_CHECK_MODULES([dfbegl], [dfbegl >= 1.7.0], 
		[
			PKG_CHECK_MODULES([egl], [$EGL_DEP], 
			[
			    REQ_PACKAGES="$REQ_PACKAGES $EGL_DEP"
				AC_DEFINE([HAVE_EGL], [1], [Enable EGL support])
				enable_egl=yes
			],
			[
			    AC_MSG_WARN([*** DirectFB EGL library not found -- Building without EGL support.])
			])
		],
		[
		    AC_MSG_WARN([*** dfbegl not found -- Building without EGL support.])
		])		
    ]
)

cairo_note=""
AS_IF([test "x$enable_cairo" = "xyes"], [
	PKG_CHECK_MODULES([Cairo], [$CAIRO_DEP], 
		[
			REQ_PACKAGES="$REQ_PACKAGES $CAIRO_DEP"
			AC_DEFINE([HAVE_CAIRO], [1], [Enable Cairo directfb support])
			AS_IF([test "x$enable_egl" = "xyes"], [
				PKG_CHECK_MODULES([CairoGLES], [cairo-glesv2], [
					cairo_note="(backends: directfb, glesv2)"
					REQ_PACKAGES="$REQ_PACKAGES $CAIRO_DEP cairo-glesv2"
					AC_DEFINE([HAVE_CAIROGLES], [1], [Enable Cairo glesv2 support])
				], [])
			], [cairo_note="(backend: directfb)"])
		],
		[
			enable_cairo=no
			AC_MSG_WARN([*** Cairo not found -- Building without Cairo support.])
		]
	)], []
)

AS_IF([test "x$enable_fusiondale" = "xyes"], [
	PKG_CHECK_MODULES([FusionDale], [$FUSIONDALE_DEP], 
		[				
			REQ_PACKAGES="$REQ_PACKAGES $FUSIONDALE_DEP"
			AC_DEFINE([HAVE_FUSIONDALE], [1], [Enable FusionDale support])
		], 
		[
			enable_fusiondale=no
			enable_compositor=no 
			AC_MSG_WARN([*** FusionDale packages not found -- Building without FusionDale support.])
		]
	)], []
)

AS_IF([test "x$enable_fusionsound" = "xyes"], [
	PKG_CHECK_MODULES([FusionSound], [$FUSIONSOUND_DEP], 
		[	
			REQ_PACKAGES="$REQ_PACKAGES $FUSIONSOUND_DEP"
			AC_DEFINE([HAVE_FUSIONSOUND], [1], [Enable FusionSound support])
		], 
		[
			enable_fusionsound=no 
			AC_MSG_WARN([*** FusionSound packages not found -- Building without FusionSound support.])
		]
	)], []
)

AS_IF([test "x$enable_sawman" = "xyes"], 
    [
        PKG_CHECK_MODULES([SaWMan], [$SAWMAN_DEP], 
        [
            REQ_PACKAGES="$REQ_PACKAGES $SAWMAN_DEP"
            AC_DEFINE([HAVE_SAWMAN], [1], [Enable SaWMan support])
        ],
        [
            enable_sawman=no
            enable_compositor=no
            AC_MSG_WARN([*** SaWMan not found -- Building without SaWMan support.])
        ])
    ]
)

LIBREFLEX=""
AS_IF([test "x$enable_reflex" = "xyes"], 
    [
        AC_CHECK_HEADER([Reflex/Reflex.h], 
        [
            LIBREFLEX="-lReflex"
            AC_DEFINE([HAVE_LIBREFLEX], [1], [Enable Reflex support])
        ],
        [
            enable_reflex=no
            AC_MSG_WARN([*** Reflex not found -- Building without Reflex support.])
        ])
    ]
)

LIBWNN=""
AS_IF([test "x$enable_wnn" = "xyes"], 
    [
        AC_CHECK_HEADER([wnn/jllib.h], 
        [
            LIBWNN="-lwnn -lcrypt"
            AC_DEFINE([HAVE_LIBWNN], [1], [Enable Wnn support])
        ],
        [
            enable_wnn=no
            AC_MSG_WARN([*** Wnn not found -- Building without Wnn support.])
        ])
    ]
)

AS_IF([test "x$enable_nls" = "xyes"], 
    [
        AC_CHECK_HEADER([libintl.h], 
        [
        	AM_GNU_GETTEXT_VERSION(0.17)
        	AM_GNU_GETTEXT([external])
            AC_DEFINE([HAVE_NLS], [1], [Enable NLS support])
        ],
        [
            enable_nls=no
            AC_MSG_WARN([*** libintl not found -- Building without NLS support.])
        ])
    ]
)

enable_sip=no
AC_CHECK_HEADER([baresip_dale.h], [enable_sip=yes], [AC_MSG_WARN([*** Baresip not found!])], [])

PKG_CHECK_MODULES(DEPS, $REQ_PACKAGES)
AC_SUBST(REQ_PACKAGES)

######################################################################
# Swig wrappers
######################################################################
WRAPPERS=
AC_SUBST(WRAPPERS)
if test -z "$SWIG" ; then
	with_csharp=no
fi

# CSharp
csc_note=
if test no != "$with_csharp" ; then
	AC_MSG_CHECKING([CSharp compiler])
	AC_ARG_VAR(CSC, [CSharp compiler command])
	if test -n "$CSC" ; then
		AC_PATH_PROGS(CSC, ["$CSC"], [])
	fi
	
	if test -z "$CSC" ; then
    	AC_PATH_PROGS(CSC, [cli-csc mono-csc], [])
    	if test -z "$CSC" ; then
			AC_PATH_PROGS(CSC, [mcs], [])
			if test -n "$CSC" ; then
				AC_MSG_CHECKING([whether $CSC is from GNU Mono])
				if (exec >&5 2>&5; $CSC --version </dev/null;exit $?) ; then
	  				AC_MSG_RESULT(yes)
				else
	  				AC_MSG_RESULT(no)
	  				CSC=
				fi
      		fi
    	fi
    	
		if test -z "$CSC" ; then
      		AC_PATH_PROGS(CSC, [dmcs smcs gmcs cscc], [])
    	fi
    	
    	if test -z "$CSC" ; then
      		AC_PATH_PROGS(CSC, [csc], [])
      		if test -n "$CSC" ; then
				AC_MSG_CHECKING([whether $CSC is for CSharp])
				if $CSC -version 2>/dev/null|grep chicken > /dev/null ; then
	  				AC_MSG_RESULT(no)
	  				CSC=
				else
	  				AC_MSG_RESULT(yes)
				fi
      		fi
    	fi		  				
    fi
    
    if test -z "$CSC" ; then
    	with_csharp=no
    	csc_note="- CSharp compiler not found!"
    else
    	WRAPPERS="$WRAPPERS csharp"
		csc_note="(CSC: $CSC)"
	fi
fi

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
DEPS_LIBS="$DEPS_LIBS $LIBREFLEX $LIBWNN"
CXXFLAGS="$CXXFLAGS $CFLAGS"

with_surface_events=no
if test -n "$DFB_SURFACE_EVENTS"; then
    with_surface_events=$DFB_SURFACE_EVENTS
else
    if $PKG_CONFIG --atleast-version=1.6.0 directfb; then
        with_surface_events=yes
    else
        enable_compositor=no
    fi
fi

if test "x$enable_compositor" = "xyes"; then
    AC_DEFINE([HAVE_COMPOSITOR], [1], [Enable Compositor support])
fi

if test "x$enable_wstring" = "xyes"; then
    AC_DEFINE([USE_WSTRING], [1], [Enable wstring])
fi

if test "x$with_focus_overlays" = "xyes"; then
    AC_DEFINE([HAVE_FOCUS_OVERLAY], [1], [Draws focus overlay images])
fi

AC_DEFINE_UNQUOTED([BINDIR], ["$prefix/bin/"], [ilixi binary directory])
AC_DEFINE_UNQUOTED([DATADIR], ["$prefix/share/$PACKAGE-$VERSION/"], [ilixi data directory])
AC_DEFINE_UNQUOTED([IMAGEDIR], ["$prefix/share/$PACKAGE-$VERSION/images/"], [ilixi image directory])

AM_CONDITIONAL([WITH_COMPOSITOR], [test x$enable_compositor = xyes])
AM_CONDITIONAL([WITH_FUSIONDALE], [test x$enable_fusiondale = xyes])
AM_CONDITIONAL([WITH_FUSIONSOUND], [test x$enable_fusionsound = xyes])
AM_CONDITIONAL([WITH_SAWMAN], [test x$enable_sawman = xyes])
AM_CONDITIONAL([WITH_REFLEX], [test x$enable_reflex = xyes])
AM_CONDITIONAL([WITH_NLS], [test x$enable_nls = xyes])
AM_CONDITIONAL([WITH_WNN], [test x$enable_wnn = xyes])
AM_CONDITIONAL([WITH_DEMOS], [test x$with_demos = xyes])
AM_CONDITIONAL([WITH_EXAMPLES], [test x$with_examples = xyes])
AM_CONDITIONAL([WITH_SURFACE_EVENTS], [test x$with_surface_events = xyes])
AM_CONDITIONAL([WITH_BARESIP], [test x$enable_sip = xyes])
AM_CONDITIONAL([WITH_EGL], [test "$enable_egl" = "yes"])
AM_CONDITIONAL([WITH_CAIRO], [test "$enable_cairo" = "yes"])
AM_CONDITIONAL([WITH_CSHARP], [test "$with_csharp" = "yes"])

ILIXI_DATADIR=$prefix/share/$PACKAGE-$VERSION
AC_SUBST(ILIXI_DATADIR)

AC_CONFIG_FILES([ \
        Makefile  \
        apps/cairo/Makefile \
        apps/calc/Makefile \
        apps/car/appdefs/Makefile \
        apps/car/ccompositor/images/Makefile \
        apps/car/ccompositor/Makefile \
        apps/car/home/Makefile \
        apps/car/Makefile \
        apps/car/statusbar/images/Makefile \
        apps/car/statusbar/Makefile \
        apps/carousel/Makefile \
        apps/demo2/Makefile \
        apps/demo4/Makefile \
        apps/egl/Makefile \
        apps/gallery/images/Makefile \
        apps/gallery/Makefile \
        apps/gestures/Makefile \
        apps/Makefile \
        apps/monitor/Makefile \
        apps/osk/images/Makefile \
        apps/osk/Makefile \
        apps/phone/appdefs/Makefile \
        apps/phone/dialer/Makefile \
        apps/phone/home/Makefile \
        apps/phone/Makefile \
        apps/phone/pcompositor/Makefile \
        apps/phone/statusbar/images/Makefile \
        apps/phone/statusbar/Makefile \
        apps/player/Makefile \
        apps/settings/Makefile \
        apps/sip/Makefile \
        apps/soundmixer/images/Makefile \
        apps/soundmixer/Makefile \
        apps/stacking/Makefile \
        apps/widgets/Makefile \
        data/apps/icons/Makefile \
        data/apps/Makefile \
        data/ilixi_catalog.xml \
        data/images/Makefile \
        data/sounds/Makefile \
        data/Makefile \
        data/themes/dark/Makefile \
        data/themes/default/Makefile \
        data/themes/Makefile \
        doc/Makefile \
        doc/resources/Makefile \
        doc/uml/Makefile \
        examples/animations/Makefile \
        examples/Makefile \
        ilixi.pc:ilixi.pc.in
        ilixi/compositor/Makefile \
        ilixi/core/Makefile \
        ilixi/graphics/Makefile \
        ilixi/lib/Makefile \
        ilixi/Makefile \
        ilixi/types/Makefile \
        ilixi/ui/Makefile \
        swig/Makefile \
        swig/csharp/Makefile \
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
   Debug enabled            : $enable_debug
   Trace enabled            : $enable_trace

   Compositor support       : $enable_compositor
   Surface event support    : $with_surface_events
   FusionDale support       : $enable_fusiondale
   FusionSound support      : $enable_fusionsound
   Sawman support           : $enable_sawman
   Reflex support           : $enable_reflex
   Wnn support              : $enable_wnn
   NLS support              : $enable_nls
   Stereoscopy support      : $enable_stereoscopy   
   EGL support              : $enable_egl
   Cairo painter            : $enable_cairo $cairo_note
   CSharp bindings          : $with_csharp $csc_note
      
   Examples                 : $with_examples
   Demos                    : $with_demos  

Type \`make' to build ilixi. 
Type \`make install' to install ilixi.
]);
