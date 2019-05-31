AC_PREREQ(2.61)
AC_INIT([verity-utils],1.0.0)
AM_INIT_AUTOMAKE([-Wall gnu foreign])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
  AM_PROG_AS
  AC_PROG_CC
  AM_PROG_CC_C_O
  AC_PROG_LIBTOOL
  AC_PROG_AWK
  AC_PROG_CPP
  AC_PROG_CXX
  AC_PROG_INSTALL
  AC_PROG_LN_S
  AC_PROG_MAKE_SET

#check for various packages.

  PKG_CHECK_MODULES([SPARSE],[libsparse],,[AC_MSG_ERROR([the requested libsparse is not found on the system])])
  AC_SUBST([SPARSE_LIBS])
  AC_SUBST([SPARSE_CFLAGS])


  PKG_CHECK_MODULES([Z],[zlib],,[AC_MSG_ERROR([the requested libz is not found on the system])])
  AC_SUBST([Z_LIBS])
  AC_SUBST([Z_CFLAGS])


  AC_ARG_WITH([coreheader_includes],AC_HELP_STRING([--with-coreheader-includes=@<:@dir@:>@],[Specify the location of the included headers]),[coreheader_incdir=$withval],[with_coreheader_includes=no])
  if test "x$with_coreheader_includes" != "xno"; then
    AC_SUBST(core_incdir,[${coreheader_incdir}])
  fi

  AC_ARG_WITH([mkbootimgheader_includes],AC_HELP_STRING([--with-mkbootimgheader-includes=@<:@dir@:>@],[Specify the location of the included headers]),[mkbootimgheader_incdir=$withval],[with_mkbootimgheader_includes=no])
  if test "x$with_mkbootimgheader_includes" != "xno"; then
    AC_SUBST(MKBOOTIMG_CFLAGS,[${mkbootimgheader_incdir}])
  fi


  AC_ARG_WITH([ext4utils_includes],AC_HELP_STRING([--with-ext4utils-includes=@<:@dir@:>@],[Specify the location of the included headers]),[ext4utilsheader_incdir=$withval],[with_ext4utils_includes=no])
  if test "x$with_ext4utils_includes" != "xno"; then
    AC_SUBST(EXT4UTILS_CFLAGS,[${ext4utilsheader_incdir}])
  fi
 

  AC_OUTPUT([ \
          Makefile
  ])
