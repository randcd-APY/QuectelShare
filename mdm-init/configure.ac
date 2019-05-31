#                                               -*- Autoconf -*-
#
# configure.ac -- Autoconf script for device-krypton.
#

# Process this file with autoconf to produce a configure script

# Requires autoconf tool later than 2.61
AC_PREREQ(2.61)
# Initialize the ath6kl package version 1.0.0
AC_INIT([device-krypton],1.0.0)
# Does not strictly follow GNU Coding standards
AM_INIT_AUTOMAKE([foreign])
# Disables auto rebuilding of configure, Makefile.ins
AM_MAINTAINER_MODE
# defines some macros variable to be included by source
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_ARG_ENABLE([target-mdm9607],
	AC_HELP_STRING([--enable-target-mdm9607],
		[Enable conditional compile for target mdm9607]),
	[target_mdm9607="${enableval}"],
	target_mdm9607=no)

AC_ARG_ENABLE([target-apq8096],
	AC_HELP_STRING([--enable-target-apq8096],
		[Enable conditional compile for target apq8096]),
	[target_apq8096="${enableval}"],
	target_apq8096=no)

AC_ARG_ENABLE([target-apq8098],
	AC_HELP_STRING([--enable-target-apq8098],
		[Enable conditional compile for target apq8098]),
	[target_apq8098="${enableval}"],
	target_apq8098=no)

AC_ARG_ENABLE([target-msm8909],
	AC_HELP_STRING([--enable-target-msm8909],
		[Enable conditional compile for target msm8909]),
	[target_msm8909="${enableval}"],
	target_msm8909=no)

AC_ARG_ENABLE([target-mdm9650],
	AC_HELP_STRING([--enable-target-mdm9650],
		[Enable conditional compile for target mdm9650]),
	[target_mdm9650="${enableval}"],
	target_mdm9650=no)

AC_ARG_ENABLE([target-apq8017],
	AC_HELP_STRING([--enable-target-apq8017],
		[Enable conditional compile for target apq80017]),
	[target_apq8017="${enableval}"],
	target_apq8017=no)

AC_ARG_ENABLE([target-qcs605],
	AC_HELP_STRING([--enable-target-qcs605],
		[Enable conditional compile for target qcs605]),
	[target_qcs605="${enableval}"],
	target_qcs605=no)

AC_ARG_ENABLE([pronto-wlan],
	AC_HELP_STRING([--enable-pronto-wlan],
		[Enable pronto wlan compile for targets that support Pronto]),
	[pronto_wlan="${enableval}"],
	pronto_wlan=no)

AC_ARG_ENABLE([drone-wlan],
	AC_HELP_STRING([--enable-drone-wlan],
		[Enable drone wlan compile for targets that support drone]),
	[drone_wlan="${enableval}"],
	drone_wlan=no)

AC_ARG_ENABLE([snap-wlan],
	AC_HELP_STRING([--enable-snap-wlan],
		[Enable snap wlan compile for targets that support snap]),
	[snap_wlan="${enableval}"],
	snap_wlan=no)

AC_ARG_ENABLE([quec-smart-wlan],
	AC_HELP_STRING([--enable-quec-smart-wlan],
		[Enable quec smart wlan compile for targets that support robot som]),
	[quec_smart_wlan="${enableval}"],
	quec_smart_wlan=no)

AC_ARG_ENABLE([robot-wlan],
	AC_HELP_STRING([--enable-robot-wlan],
		[Enable robot wlan compile for targets that support robot]),
	[robot_wlan="${enableval}"],
	robot_wlan=no)

AC_ARG_ENABLE([naples-wlan],
	AC_HELP_STRING([--enable-naples-wlan],
		[Enable naples wlan compile for targets that support naples]),
	[naples_wlan="${enableval}"],
	naples_wlan=no)

AC_ARG_ENABLE([target-sdx20],
       AC_HELP_STRING([--enable-target-sdx20],
               [Enable conditional compile for target sdx20]),
       [target_sdx20="${enableval}"],
       target_sdx20=no)

AC_ARG_ENABLE([target-sdxpoorwills],
	AC_HELP_STRING([--enable-target-sdxpoorwills],
		[Enable conditional compile for target sdxpoorwills]),
	[target_sdxpoorwills="${enableval}"],
	target_sdxpoorwills=no)

AC_ARG_ENABLE([target-qcs405-som1],
	AC_HELP_STRING([--enable-target-qcs405-som1],
		[Enable conditional compile for target qcs405-som1]),
	[target_qcs405som1="${enableval}"],
	target_qcs405som1=no)

AC_ARG_ENABLE([ipc-wlan],
	AC_HELP_STRING([--enable-ipc-wlan],
		[Enable ipc wlan compile for targets that support ipc]),
	[ipc_wlan="${enableval}"],
	ipc_wlan=no)

AC_ARG_ENABLE([thirdparty-wlan],
	AC_HELP_STRING([--enable-thirdparty-wlan],
		[Enable conditional wlan compile for third-party target]),
	[thirdparty_wlan="${enableval}"],
	thirdparty_wlan=no)

AM_CONDITIONAL([IS_MDM9607], [test "x$target_mdm9607" = xyes])

AM_CONDITIONAL([IS_APQ8098], [test "x$target_apq8098" = xyes])

AM_CONDITIONAL([NEED_SDIO_INI], [test "x$target_sdx20" = xyes -o "x$target_mdm9650" = xyes -o "x$target_sdxpoorwills" = xyes -o "x$thirdparty_wlan" = xyes])

AM_CONDITIONAL([IS_PRONTO_ENABLED], [test "x$pronto_wlan" = xyes])

AM_CONDITIONAL([IS_MDM9650], [test "x$target_mdm9650" = xyes])

AM_CONDITIONAL([IS_SDX20], [test "x$target_sdx20" = xyes])

AM_CONDITIONAL([IS_QCS605], [test "x$target_qcs605" = xyes])

AM_CONDITIONAL([IS_SDXPOORWILLS], [test "x$target_sdxpoorwills" = xyes])

AM_CONDITIONAL([IS_QCS405SOM1], [test "x$target_qcs405som1" = xyes])

AM_CONDITIONAL([IS_THIRDPARTY_WLAN], [test "x$thirdparty_wlan" = xyes])

if (test "x${drone_wlan}" = "xyes"); then
	AM_CONDITIONAL([IS_APQ8096_DRONE], [test "x$target_apq8096" = xyes])
	AM_CONDITIONAL([IS_APQ8009_DRONE], [test "x$target_apq8009" = xyes])
else
	AM_CONDITIONAL([IS_APQ8096_DRONE], [test "x$drone_wlan" = xyes])
	AM_CONDITIONAL([IS_APQ8009_DRONE], [test "x$drone_wlan" = xyes])
fi

if (test "x${snap_wlan}" = "xyes"); then
	AM_CONDITIONAL([IS_APQ8017_SNAP], [test "x$target_apq8017" = xyes])
else
	AM_CONDITIONAL([IS_APQ8017_SNAP], [test "x$snap_wlan" = xyes])
fi
AM_CONDITIONAL([IS_IPC], [test "x$ipc_wlan" = xyes])

if (test "x${robot_wlan}" = "xyes"); then
	AM_CONDITIONAL([IS_APQ8009_ROBOT], [test "x$target_apq8009" = xyes])
else
	AM_CONDITIONAL([IS_APQ8009_ROBOT], [test "x$robot_wlan" = xyes])
fi

if (test "x${quec_smart_wlan}" = "xyes"); then
	AM_CONDITIONAL([IS_MSM8909_QUEC_SMART], [test "x$target_msm8909" = xyes])
else
	AM_CONDITIONAL([IS_MSM8909_QUEC_SMART], [test "x$quec_smart_wlan" = xyes])
fi

if (test "x${naples_wlan}" = "xyes"); then
	AM_CONDITIONAL([IS_APQ8009_NAPLES], [test "x$target_apq8009" = xyes])
else
	AM_CONDITIONAL([IS_APQ8009_NAPLES], [test "x$naples_wlan" = xyes])
fi

AC_CONFIG_FILES([Makefile])

AM_COND_IF([IS_MDM9607],
	[AC_CONFIG_FILES([wlan_sdio/Makefile])],
	[AC_CONFIG_FILES([wlan/Makefile])])
AC_OUTPUT

AM_COND_IF([IS_PRONTO_ENABLED],
	[AC_CONFIG_FILES([wlan_pronto/Makefile])])
AC_OUTPUT

AM_COND_IF([IS_APQ8009_ROBOT],
	[AC_CONFIG_FILES([wlan_apq8009_robot/Makefile])])
AC_OUTPUT

AM_COND_IF([IS_MSM8909_QUEC_SMART],
	[AC_CONFIG_FILES([wlan_msm8909_quec_smart/Makefile])])
AC_OUTPUT

AM_COND_IF([IS_APQ8009_NAPLES],
	[AC_CONFIG_FILES([wlan_apq8009_qsap/Makefile])])
AC_OUTPUT

AM_COND_IF([IS_APQ8096_DRONE],
	[AC_CONFIG_FILES([wlan_apq8096_drone/Makefile])])
AC_OUTPUT

AM_COND_IF([IS_APQ8098],
	[AC_CONFIG_FILES([wlan_apq8098_mediabox/Makefile])])
AC_OUTPUT

AM_COND_IF([IS_QCS605],
	[AC_CONFIG_FILES([wlan_qcs605_concam/Makefile])])
AC_OUTPUT

AM_COND_IF([IS_APQ8009_DRONE],
	[AC_CONFIG_FILES([wlan_apq8009_drone/Makefile])])
AC_OUTPUT

AM_COND_IF([IS_APQ8017_SNAP],
	[AC_CONFIG_FILES([wlan_apq8017_snap/Makefile])])
AC_OUTPUT

AM_COND_IF([IS_IPC],
	[AC_CONFIG_FILES([wlan_ipc/Makefile])])
AC_OUTPUT

AM_COND_IF([IS_MDM9650],
	[AC_CONFIG_FILES([wlan_mdm9650/Makefile])])
AC_OUTPUT

AM_COND_IF([IS_SDX20],
	[AC_CONFIG_FILES([wlan_sdx20/Makefile])])
AC_OUTPUT

AM_COND_IF([IS_SDXPOORWILLS],
	[AC_CONFIG_FILES([wlan_sdxpoorwills/Makefile])])
AC_OUTPUT

AM_COND_IF([IS_QCS405SOM1],
	[AC_CONFIG_FILES([wlan_qcs405-som1_qsap/Makefile])])
AC_OUTPUT

AM_COND_IF([IS_THIRDPARTY_WLAN],
	[AC_CONFIG_FILES([wlan_standalone/Makefile])])
AC_OUTPUT
