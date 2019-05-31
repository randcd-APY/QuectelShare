inherit autotools-brokensep qcommon pkgconfig
require ${COREBASE}/meta-qti-location/recipes/include/common-location-defines.inc

DESCRIPTION = "Loc API"
PR = "r1"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/\
${LICENSE};md5=3775480a712fc46a69647678acb234cb"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://qcom-opensource/location/loc_api/loc_api_v02/"
SRC_DIR = "${WORKSPACE}/qcom-opensource/location/loc_api/loc_api_v02/"
S = "${WORKDIR}/qcom-opensource/location/loc_api/loc_api_v02"

DEPENDS = "system-core loc-core loc-ds"
EXTRA_OECONF = "--with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-glib"

CPPFLAGS += "-I${WORKSPACE}/base/include"
