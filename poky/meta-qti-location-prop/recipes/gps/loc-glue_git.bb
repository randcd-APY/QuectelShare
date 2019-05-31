inherit autotools-brokensep pkgconfig qcommon qlicense qprebuilt utils
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "GPS Location Service library"
PR = "r1"

FILESPATH =+ "${WORKSPACE}/gps/framework/native:"
SRC_URI = "file://framework-glue"
SRC_DIR = "${WORKSPACE}/gps/framework/native/framework-glue"
S = "${WORKDIR}/framework-glue"

DEPENDS = "location-geofence location-flp location-service"

DISABLE_QCMAP = 'False'
DISABLE_QCMAP = "${@bb.utils.contains('BASEMACHINE', '8x96autogvmquintcu', 'True', 'DISABLE_QCMAP',d)}"
# don't include loc-net-iface for 8x96autogvmquintcu
DEPENDS += "${@bb.utils.contains('DISABLE_QCMAP', 'True', '', 'loc-net-iface',d)}"

EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib \
                --enable-target=${BASEMACHINE} \
                "
