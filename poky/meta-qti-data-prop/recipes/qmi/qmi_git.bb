inherit qcommon qlicense qprebuilt

DESCRIPTION = "Qualcomm MSM Interface (QMI) Library"

PR = "r13"
SRC_DIR = "${WORKSPACE}/qmi"
S = "${WORKDIR}/qmi"

DEPENDS = "configdb diag dsutils libcutils"

CFLAGS += "${CFLAGS_EXTRA}"
CFLAGS += "-I${STAGING_INCDIR}/cutils"
CFLAGS_EXTRA_append_arm = " -fforward-propagate"
CFLAGS_append_mdm9650 += "-DFEATURE_QMUXD_DISABLED"

EXTRA_OECONF = "--with-qxdm \
                --with-common-includes=${STAGING_INCDIR}"

EXTRA_OECONF_append_msm8960 = " --enable-auto-answer=yes"
