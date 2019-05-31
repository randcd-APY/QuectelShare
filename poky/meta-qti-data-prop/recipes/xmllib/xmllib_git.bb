inherit qcommon qlicense qprebuilt

DESCRIPTION = "QTI XML Library"
HOMEPAGE = "http://support.cdmatech.com"
DEPENDS = "common diag glib-2.0"
PR = "r9"

EXTRA_OECONF = "--with-common-includes=${STAGING_INCDIR} \
                --with-glib"

SRC_DIR = "${WORKSPACE}/xmllib"
S = "${WORKDIR}/xmllib"
