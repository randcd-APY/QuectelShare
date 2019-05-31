
AC_PREREQ(2.61)

AC_INIT([libgbm],
    version-1.0,
    [],
    libgbm)

AM_INIT_AUTOMAKE([-Wall -Wno-error foreign])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
LT_INIT
AM_MAINTAINER_MODE

AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])


AC_ARG_WITH([core_includes],
    AC_HELP_STRING([--with-core-includes=@<:@dir@:>@],
       [Specify the location of the core headers]),
    [core_incdir=$withval],
    with_core_includes=no)

if test "x$with_core_includes" != "xno"; then
   CFLAGS="${CFLAGS} -I${core_incdir}"
fi

#pkgconfigdir=${libdir}/pkgconfig
#AC_SUBST(pkgconfigdir)

AC_ARG_WITH(sanitized-headers,
   AS_HELP_STRING([--with-sanitized-headers=DIR],
       [Specify the location of the sanitized Linux headers]),
   [CPPFLAGS="$CPPFLAGS -idirafter $withval"])

AC_ARG_ENABLE([compilewithdrm],
   AC_HELP_STRING([--enable-compilewithdrm],
      [enable compilewithdrm, build display-hal-linux with compilewithdrm]),
      [enable_compilewithdrm="${enableval}"],
      enable_compilewithdrm=no)
AM_CONDITIONAL([ENABLE_COMPILEWITHDRM], [test "x${enable_compilewithdrm}" = "xyes"])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

#PKG_CHECK_MODULES(LIBDRM, [libdrm])

AC_ARG_WITH(kernel-headers,
        AC_HELP_STRING([--with-kernel-headers=DIR],
               [Kernel header directory] ),
        [CFLAGS="$CFLAGS -I$withval -I${withval}/../arch/arm/include -I${withval}/include"])

ALLOCATE_SURFACE_BO_AT_CREATION=yes

AM_CONDITIONAL(ALLOCATE_SURFACE_BO_AT_CREATION, test $ALLOCATE_SURFACE_BO_AT_CREATION = yes)

if test $ALLOCATE_SURFACE_BO_AT_CREATION = yes; then
        AC_DEFINE(ALLOCATE_SURFACE_BO_AT_CREATION, 1, [Enable surface's BO creation at surface creation time])
fi


BUILD_HAS_WAYLAND_SUPPORT=no

AM_CONDITIONAL(BUILD_HAS_WAYLAND_SUPPORT, test $BUILD_HAS_WAYLAND_SUPPORT = yes)

if test $BUILD_HAS_WAYLAND_SUPPORT = yes; then
        AC_DEFINE(BUILD_HAS_WAYLAND_SUPPORT, 1, [Enable when build has wayland support])
fi

AC_CONFIG_FILES([ \
    Makefile \
    gbm.pc
    ])

AC_SUBST([CFLAGS])
AC_SUBST([CPPFLAGS])
AC_SUBST([LIBS])

AC_OUTPUT
#AC_OUTPUT([Makefile src/Makefile])
