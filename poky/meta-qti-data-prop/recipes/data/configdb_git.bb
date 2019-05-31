inherit qcommon qlicense qprebuilt autotools

DESCRIPTION = "Qualcomm Data Configdb Module"
DEPENDS = "common dsutils diag xmllib glib-2.0"
PR = "r5"

EXTRA_OECONF = "--with-lib-path=${STAGING_LIBDIR} \
                --with-common-includes=${STAGING_INCDIR} \
                --with-glib \
                --with-qxdm"

FILESPATH =+ "${WORKSPACE}/data:"
SRC_URI = "file://configdb"
SRC_DIR = "${WORKSPACE}/data/configdb"

S       = "${WORKDIR}/configdb"
