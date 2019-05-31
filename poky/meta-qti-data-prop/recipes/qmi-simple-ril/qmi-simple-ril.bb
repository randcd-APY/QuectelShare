inherit autotools qcommon qlicense qprebuilt

DESCRIPTION = "QMI Test application"

PR = "r1"

FILESPATH =+ "${WORKSPACE}/qmi:"
SRC_URI = "file://tests"
SRC_DIR = "${WORKSPACE}/qmi/tests"
S = "${WORKDIR}/tests"

DEPENDS = "qmi qmi-framework"

CFLAGS += "${CFLAGS_EXTRA}"
CFLAGS_EXTRA_append_arm = " -fforward-propagate"

EXTRA_OECONF = "--with-common-includes=${STAGING_INCDIR}"

