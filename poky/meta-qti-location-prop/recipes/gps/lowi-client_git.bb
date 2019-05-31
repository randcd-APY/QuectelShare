inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc
DESCRIPTION = "GPS lowi client"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps-noship/internal/lowi/"
SRC_DIR = "${WORKSPACE}/gps-noship/internal/lowi/"
S = "${WORKDIR}/gps-noship/internal/lowi"

DEPENDS = "common loc-mq-client"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-libhardware-includes=${STAGING_INCDIR} \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"
do_install_append() {
if [ -d "${SRC_DIR}" ]; then
   install -d ${D}/usr/include
   install -d ${D}/usr/include/lowi
   install -d ${D}/usr/include/lowi/inc
   install -m 0644 ${S}/inc/lowi_client.h ${D}/usr/include/lowi/inc
   install -m 0644 ${S}/inc/lowi_client_receiver.h ${D}/usr/include/lowi/inc
   install -m 0644 ${S}/inc/lowi_const.h ${D}/usr/include/lowi/inc
   install -m 0644 ${S}/inc/lowi_defines.h ${D}/usr/include/lowi/inc
   install -m 0644 ${S}/inc/lowi_mac_address.h ${D}/usr/include/lowi/inc
   install -m 0644 ${S}/inc/lowi_request.h ${D}/usr/include/lowi/inc
   install -m 0644 ${S}/inc/lowi_response.h ${D}/usr/include/lowi/inc
   install -m 0644 ${S}/inc/lowi_scan_measurement.h ${D}/usr/include/lowi/inc
   install -m 0644 ${S}/inc/lowi_ssid.h ${D}/usr/include/lowi/inc
else
   qprebuilt_do_install
fi
}
