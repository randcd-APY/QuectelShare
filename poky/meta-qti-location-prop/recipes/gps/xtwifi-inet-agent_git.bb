inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "GPS xtwifi tile"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps-noship/xtwifi/inet_agent/src/"
SRC_DIR = "${WORKSPACE}/gps-noship/xtwifi/inet_agent/src/"
S = "${WORKDIR}/gps-noship/xtwifi/inet_agent/src"

DEPENDS = "curl loc-mq-client"
EXTRA_OECONF = "--with-core-includes=${WORKSPACE}/system/core/include \
                --with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"
