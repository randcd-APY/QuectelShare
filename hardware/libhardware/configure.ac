AC_PREREQ([2.65])
AC_INIT([libhardware],1.0.0)
AM_INIT_AUTOMAKE([-Wall gnu foreign])
AC_CONFIG_SRCDIR([hardware.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

AC_CONFIG_FILES([Makefile \
        hardware.pc
        ])

AC_PROG_CXX
AC_PROG_CC

LT_INIT

AC_CHECK_HEADERS([fcntl.h limits.h stdint.h stdlib.h string.h sys/ioctl.h unistd.h])

AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_CHECK_FUNCS([memset munmap strerror])

AC_ARG_ENABLE(audio,
      AC_HELP_STRING([--disable-audio], [Disable audio header installation]),
        [has_audio=${enableval}], [has_audio="yes"])

AM_CONDITIONAL(HAS_AUDIO, [test "x${has_audio}" = "xyes"])

AC_ARG_ENABLE(camera,
      AC_HELP_STRING([--disable-camera], [Disable camera header installation]),
        [has_camera=${enableval}], [has_camera="no"])

AM_CONDITIONAL(HAS_CAMERA, [test "x${has_camera}" = "xyes"])

AC_ARG_ENABLE(display,
      AC_HELP_STRING([--disable-display], [Disable display header installation]),
        [has_display=${enableval}], [has_display="yes"])

AM_CONDITIONAL(HAS_DISPLAY, [test "x${has_display}" = "xyes"])

AC_ARG_ENABLE(location,
      AC_HELP_STRING([--disable-location], [Disable location header installation]),
        [has_location=${enableval}], [has_location="yes"])

AM_CONDITIONAL(HAS_LOCATION, [test "x${has_location}" = "xyes"])

AC_ARG_ENABLE(sensors,
      AC_HELP_STRING([--disable-sensors], [Disable sensor header installation]),
        [has_sensors=${enableval}], [has_sensors="no"])

AM_CONDITIONAL(HAS_SENSORS, [test "x${has_sensors}" = "xyes"])

AC_OUTPUT
