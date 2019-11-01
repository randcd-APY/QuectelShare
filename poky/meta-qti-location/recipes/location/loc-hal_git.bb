inherit autotools-brokensep qcommon pkgconfig
require ${COREBASE}/meta-qti-location/recipes/include/common-location-defines.inc

DESCRIPTION = "GPS Loc HAL"
PR = "r5"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/\
${LICENSE};md5=3775480a712fc46a69647678acb234cb"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://hardware/qcom/gps/"
SRC_DIR = "${WORKSPACE}/hardware/qcom/gps/"
S = "${WORKDIR}/hardware/qcom/gps"

DEPENDS = "data qmi qmi-framework loc-core loc-flp-hdr loc-pla loc-stub"
EXTRA_OECONF = "--with-locpla-includes=${STAGING_INCDIR}/loc-pla \
                --with-core-includes=${WORKSPACE}/system/core/include \
                --with-glib"

CPPFLAGS += "-I${WORKSPACE}/base/include"

PACKAGES = "${PN}"
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
FILES_${PN} = "${libdir}/* ${sysconfdir}"
FILES_${PN} += "/usr/include/*"
FILES_${PN} += "/usr/include/loc-hal/*"
INSANE_SKIP_${PN} = "dev-so"

do_install_append() {
   install -m 0644 -D ${S}/etc/gps.conf ${D}${sysconfdir}/gps.conf
}
