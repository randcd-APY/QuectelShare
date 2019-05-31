FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI = "${@bb.utils.contains('DISTRO_FEATURES','ab-boot-support','file://set-slotsuffix.service','',d)}"

do_install_append() {
    install -d ${D}${systemd_unitdir}/system
    if ${@bb.utils.contains('DISTRO_FEATURES', 'ab-boot-support', 'true', 'false', d)}; then
        install -m 0644 ${WORKDIR}/set-slotsuffix.service ${D}${systemd_unitdir}/system
    fi

}

SYSTEMD_SERVICE_${PN} += "${@bb.utils.contains('DISTRO_FEATURES','ab-boot-support',' set-slotsuffix.service','',d)}"
