inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "SLIM daemon "
PR = "r1"

SRC_URI = "file://gps/slim/"
SRC_DIR = "${WORKSPACE}/gps/slim/"
S = "${WORKDIR}/gps/slim"

DEPENDS = "canwrapper libsensors slim-client"
EXTRA_OECONF = "--with-core-includes=${STAGING_INCDIR} \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib \
                --enable-target=${BASEMACHINE} \
                --enable-vnw=yes"
S = "${WORKDIR}/gps/slim"

do_install_append () {
    chmod 4755 ${D}${bindir}/slim_daemon
}
