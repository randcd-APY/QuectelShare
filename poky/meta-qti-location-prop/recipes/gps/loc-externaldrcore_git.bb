inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "DR core"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps-noship/dr_amt/qdr_core/"
SRC_DIR = "${WORKSPACE}/gps-noship/dr_amt/qdr_core/"
S = "${WORKDIR}/gps-noship/dr_amt/qdr_core"

DEPENDS = "loc-hal"
EXTRA_OECONF = "--with-glib \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --enable-a6dof=yes \
               "
do_install_append () {
        if [ -d "${SRC_DIR}" ]; then
             install -d ${D}${includedir}
             install -m 644 ${S}/qdr_core_if.h ${D}${includedir}
        fi
}

