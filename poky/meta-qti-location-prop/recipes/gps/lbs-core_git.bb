inherit autotools-brokensep pkgconfig qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "GPS LBS Core"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps/framework/native/core/"
SRC_DIR = "${WORKSPACE}/gps/framework/native/core/"
S = "${WORKDIR}/gps/framework/native/core"

DEPENDS = "qmi-framework gps-utils loc-core loc-api-v02 loc-base-util loc-flp-hdr izat-core"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-locflp-includes=${STAGING_INCDIR}/loc-flp-hdr \
                --with-glib"

SRC_URI +="file://overlay/"
do_install_append() {
    #Install overlay conf file for target,if exists
    izatConfOverlayFile='${WORKDIR}/overlay/${BASEMACHINE}/etc/izat.conf'
    if [ -e ${izatConfOverlayFile} ]; then
        echo "Installing izat.conf overlay file: ${izatConfOverlayFile}"
        install -m 0644 -D "${izatConfOverlayFile}" ${D}${sysconfdir}/izat.conf
    fi
}
