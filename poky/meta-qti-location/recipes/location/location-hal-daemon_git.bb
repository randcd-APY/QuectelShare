inherit autotools qcommon qlicense qprebuilt update-rc.d systemd
require ${COREBASE}/meta-qti-location/recipes/include/common-location-defines.inc

DESCRIPTION = "location hal daemon service"
PR = "r1"

LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/\
${LICENSE};md5=3775480a712fc46a69647678acb234cb"

FILESPATH =+ "${WORKSPACE}:"
SRC_DIR = "${WORKSPACE}/qcom-opensource/location/location_hal_daemon/"
S = "${WORKDIR}/qcom-opensource/location/location_hal_daemon"

DEPENDS = "loc-pla-hdr loc-hal location-service gps-utils location-api-iface location-api"

EXTRA_OECONF = "--with-core-includes=${STAGING_INCDIR} \
                --with-locationapi-includes=${STAGING_INCDIR}/location-api-iface \
                --with-glib"

DEPENDS += "${@oe.utils.conditional('BASEMACHINE', 'mdm9607', 'powermanager-lib', '', d)}"
EXTRA_OECONF += "${@oe.utils.conditional('BASEMACHINE', 'mdm9607', '--enable-target=${BASEMACHINE}', '', d)}"

INITSCRIPT_NAME = "location_hal_initializer"
INITSCRIPT_PARAMS = "start 37 S ."

SRC_URI +="file://location_hal_daemon.service"

PACKAGES = "${PN}"
FILES_${PN} += "${libdir}/*"
FILES_${PN} += "/usr/include/*"

INSANE_SKIP_${PN} = "dev-deps"

do_install_append () {

    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -m 0755 ${S}/location_hal_initializer  -D ${D}${sysconfdir}/initscripts/location_hal_initializer
        install -d ${D}${sysconfdir}/systemd/system/
        install -m 0644 ${WORKDIR}/location_hal_daemon.service -D ${D}${sysconfdir}/systemd/system/location_hal_daemon.service
        install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants/
        ln -sf /etc/systemd/system/location_hal_daemon.service \
                          ${D}/etc/systemd/system/multi-user.target.wants/location_hal_daemon.service
    else
        install -m 0755 ${S}/location_hal_initializer -D ${D}${sysconfdir}/init.d/location_hal_initializer
    fi
}

