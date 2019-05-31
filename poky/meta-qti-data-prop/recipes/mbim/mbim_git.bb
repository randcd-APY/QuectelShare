inherit autotools qcommon qprebuilt qlicense qprebuilt

DESCRIPTION = "USB Mobile Broadband Interface Model (MBIM) Command Processor"
SRC_DIR = "${WORKSPACE}/mbim"
S = "${WORKDIR}/mbim"

DEPENDS = "diag qmi qmi-framework"
# Package Revision (update whenever recipe is changed)
PR = "r2"

EXTRA_OECONF = "--with-common-includes=${STAGING_INCDIR}"
