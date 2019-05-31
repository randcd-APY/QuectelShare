inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc
DESCRIPTION = "GPS xtwifi tile"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps-noship/xtwifi/"
SRC_DIR = "${WORKSPACE}/gps-noship/xtwifi/"
S = "${WORKDIR}/gps-noship/xtwifi"

DEPENDS = "zlib asn1c-cper gdtap-adapter lowi-client"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"

PACKAGES = "${PN}"
FILES_${PN} += "${libdir}/*.so"
FILES_${PN} += "/usr/include/*"
FILES_${PN} += "/usr/lib/*"
FILES_${PN} += "/usr/bin/*"
INSANE_SKIP_${PN} = "dev-so"
