AC_PREREQ(2.61)
AC_INIT([libcamera], 1.0.0)
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall -Wextra -Werror])
#AC_CONFIG_SRCDIR([Makefile.am])
#AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

# Check for programs
LT_INIT
AC_PROG_LIBTOOL
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
PKG_PROG_PKG_CONFIG

AS_CASE([$host],
        [arm*], [ARM=yes],
        [ARM=no]
)

ARM=yes

AC_ARG_WITH(sanitized-headers,
      AS_HELP_STRING([--with-sanitized-headers=DIR],
         [Specify the location of the sanitized Linux headers]),
      [CPPFLAGS="$CPPFLAGS -idirafter $withval"])

AC_ARG_WITH([gralloc_library],
      AC_HELP_STRING([--with-gralloc-library=@<:@dir@:>@],
         [Specify the location of the Gralloc library]),
      [gralloc_libdir=$withval],
      with_gralloc_library=no)

if test "x$with_gralloc_library" != "xno"; then
   AC_SUBST([LIBGRALLOC], "${gralloc_libdir}")
fi

AC_ARG_WITH([mm_core],
      AC_HELP_STRING([with-mm-core=@<:@dir@:>@],
         [Specify the location of the mm-core library]),
      [mm_core_libdir=$withval],
      with_mm_core_library=no)

if test "x$with_mm_core_library" != "xno"; then
   AC_SUBST([MMCORE_CFLAGS], "${mm_core_libdir}")
fi

AC_ARG_WITH([camerahal],
      AC_HELP_STRING([with-camerahal=@<:@dir@:>@],
         [Specify the location of CameraHal]),
      [camerahal_libdir=$withval],
      with_camerahal=no)

if test "x$with_camerahal" != "xno"; then
   AC_SUBST([CAMERAHAL_CFLAGS], "${camerahal_libdir}")
fi

[CPPFLAGS="$CPPFLAGS -std=c++11 -Wall -Wextra -Werror"]
# TODO functions have unused input parameters
[CPPFLAGS="$CPPFLAGS -Wno-unused-parameter"]
# TODO this warning suppress issue in lib gralloc private
[CPPFLAGS="$CPPFLAGS -Wno-multichar"]
# TODO define ANDROID macro to resolve redefinition of LOGE by MMDebugMsg.h
[CPPFLAGS="$CPPFLAGS -DANDROID"]

AC_SUBST([CFLAGS])
AC_SUBST([CPPFLAGS])
AC_SUBST([LIBS])


AC_ARG_WITH([qmmf_sdk],
      AC_HELP_STRING([--with-qmmf_sdk],
         [Build libcamera using qmmf-sdk]))

AM_CONDITIONAL(USE_QMMF, test "x${with_qmmf_sdk}" = "xyes")

AC_ARG_WITH([camhal1],
      AC_HELP_STRING([--with-camhal1],
         [Build libcamera using camera HAL 1]))

AM_CONDITIONAL(USE_CAMHAL1, test "x${with_camhal1}" = "xyes")


AC_CONFIG_FILES([ \
    Makefile \
    libcamera/Makefile \
])
AC_OUTPUT
