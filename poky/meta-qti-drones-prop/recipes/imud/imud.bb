inherit autotools qcommon qlicense systemd
DESCRIPTION = "imud"

PR = "r0"
PV = "1.0"

DEPENDS += "glib-2.0 libcutils"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://imud.c"
SRC_URI += "file://Makefile.am"
SRC_URI += "file://configure.ac"
SRC_URI += "file://imud.sh"
SRC_URI += "file://imud.service"

S = "${WORKDIR}"

do_install() {
    install -d ${D}/${sysconfdir}/init.d
    install -m 0755 ${S}/imud -D ${D}/sbin/imud
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -m 755 ${WORKDIR}/imud.sh -D ${D}/${sysconfdir}/imud.sh
        install -d ${D}${systemd_unitdir}/system/
        install -m 0644 ${WORKDIR}/imud.service -D ${D}${systemd_unitdir}/system/imud.service
        install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/
        ln -sf ${systemd_unitdir}/system/imud.service \
           ${D}${systemd_unitdir}/system/multi-user.target.wants/imud.service
    else
        install -m 755 ${WORKDIR}/imud.sh -D ${D}/${sysconfdir}/init.d/imud.sh
    fi
}

FILES_${PN} += "${systemd_unitdir}/system/"
