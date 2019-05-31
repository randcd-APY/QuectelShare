inherit update-rc.d systemd

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}-${PV}:"

DESCRIPTION = "Script to restrict kernel pointer access"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/${LICENSE};md5=550794465ba0ec5312d6919e203a55f9"

SRC_URI += "file://logging-restrictions.sh"
SRC_URI += "file://logging-restrictions.service"

PR = "r0"

do_configure[noexec] = "1"
do_compile[noexec]   = "1"

INITSCRIPT_NAME = "logging-restrictions.sh"
INITSCRIPT_PARAMS = "start 39 S ."

do_install() {
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -m 0644 ${WORKDIR}/logging-restrictions.service -D ${D}${systemd_unitdir}/system/logging-restrictions.service
        install -d ${D}${systemd_unitdir}/system/multi-user.target.wants
        ln -sf ${systemd_unitdir}/system/logging-restrictions.service \
            ${D}${systemd_unitdir}/system/multi-user.target.wants/logging-restrictions.service
    else
        install -d ${D}${sysconfdir}/kptr-restrict
        install -d ${D}${sysconfdir}/kptr-restrict/init.d
        install -m 0755 ${WORKDIR}/logging-restrictions.sh  ${D}${sysconfdir}/init.d/logging-restrictions.sh
        update-rc.d -r ${D} logging-restrictions.sh start 39 S .

    fi
}

FILES_${PN} += "${systemd_unitdir}/system/*"
