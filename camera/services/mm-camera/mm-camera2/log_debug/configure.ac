#                                               -*- Autoconf -*-
# configure.ac -- Autoconf script for mm-camera-core
#

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.69)
AC_INIT([mm-camera],1.0.0)

AM_INIT_AUTOMAKE([-Wall -Werror gnu foreign subdir-objects])
AM_MAINTAINER_MODE

AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AM_PROG_AR
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

AC_CHECK_TOOL(OBJCOPY, objcopy, false)

AC_ARG_WITH(sanitized-headers,
	    AS_HELP_STRING([--with-sanitized-headers=DIR],[Specify the location of the sanitized Linux headers]),
	    [CPPFLAGS="$CPPFLAGS -idirafter $withval"])

AC_ARG_WITH([camera-hal-path],
             AS_HELP_STRING([--with-camera-hal-path=@<:@dir@:>@],[path to camera hal]),
	     [CAMERAHAL_PATH="$withval"])

AC_ARG_WITH([mm-camera-path],
             AS_HELP_STRING([--with-mm-camera-path=@<:@dir@:>@],
	     [path to mm-camera modules]),
 	     [MM_CAMERA_PATH="$withval"])

AC_ARG_WITH([common_includes],
      AC_HELP_STRING([--with-common-includes=@<:@dir@:>@],
         [Specify the location of the common headers]),
      [common_incdir=$withval],
      with_common_includes=no)

if test "x$with_common_includes" != "xno"; then
   CFLAGS="${CFLAGS} -I${common_incdir}"
fi

AC_ARG_WITH([core_includes],
    AC_HELP_STRING([--with-core-includes=@<:@dir@:>@],
       [Specify the location of the core headers]),
    [core_incdir=$withval],
    with_core_includes=no)

if test "x$with_core_includes" != "xno"; then
   CPPFLAGS="${CPPFLAGS} -I${core_incdir}"
   AC_SUBST(core_incdir,${core_incdir})
fi

CPPFLAGS="${CPPFLAGS} -Wno-error -Wno-uninitialized -Wno-error=attributes -Wno-error=unused-parameter" 
CPPFLAGS="${CPPFLAGS} -Wno-error=builtin-macro-redefined -Wno-error=type-limits"
CPPFLAGS="${CPPFLAGS} -D_GNU_SOURCE -D_LE_CAMERA_ -DUNIX_PATH_MAX=108"
CPPFLAGS="${CPPFLAGS} -Dstrlcpy=g_strlcpy -Dstrlcat=g_strlcat"

AC_ARG_WITH([glib],
            AC_HELP_STRING([--with-glib],
            [enable glib, Build against glib. Use this when building for HLOS systems which use glib]))

if (test "x${with_glib}" = "xyes"); then
        PKG_CHECK_MODULES(GTHREAD, gthread-2.0 >= 2.16, dummy=yes,
                                AC_MSG_ERROR(GThread >= 2.16 is required))
        PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.16, dummy=yes,
				AC_MSG_ERROR(GLib >= 2.16 is required))
	GLIB_CFLAGS="$GLIB_CFLAGS $GTHREAD_CFLAGS"
        GLIB_LIBS="$GLIB_LIBS $GTHREAD_LIBS"

        AC_SUBST(GLIB_CFLAGS)
        AC_SUBST(GLIB_LIBS)
fi

AC_SUBST(MM_CAMERA_PATH)
AC_SUBST(CAMERAHAL_PATH)
AM_CONDITIONAL(USE_GLIB, test "x${with_glib}" = "xyes")

AC_OUTPUT([ \
  Makefile 
])
