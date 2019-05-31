inherit qcommon qlicense qprebuilt

DESCRIPTION = "QMI Client Helper Module"
PR = "r0"

DEPENDS = "common diag dsutils qmi-framework"

EXTRA_OECONF = "--with-common-includes=${STAGING_INCDIR} \
                --with-qxdm"

FILESPATH =+ "${WORKSPACE}/qmi:"
SRC_URI = "file://qmi_client_helper"
SRC_DIR = "${WORKSPACE}/qmi/qmi_client_helper"

S = "${WORKDIR}/qmi_client_helper"
