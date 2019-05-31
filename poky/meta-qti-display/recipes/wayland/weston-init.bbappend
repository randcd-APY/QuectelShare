SUMMARY = "Startup script for the Weston Wayland compositor"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/LICENSE;md5=4d92cd373abda3937c2bc47fbc49d690"
S = "${WORKDIR}"
DISTRO_FEATURES_append = " opengl"
FILESEXTRAPATHS_append := ":${THISDIR}/${PN}"

SRC_URI_append = "\
    file://init_qti \
    file://init_qti.service \
"

SRC_URI_append_qcs605 += "file://init_qti-qcs605.service"
DISPLAY_SERVICE_FILENAME = "init_qti.service"
DISPLAY_SERVICE_FILENAME_qcs605 = "init_qti-qcs605.service"

do_install_append() {
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -m 0755 ${S}/init_qti -D ${D}${sysconfdir}/initscripts/init_qti_display
        install -d ${D}/etc/systemd/system/
        install -m 0755 ${S}/${DISPLAY_SERVICE_FILENAME} -D ${D}${sysconfdir}/systemd/system/init_display.service
        install -d ${D}/etc/systemd/system/multi-user.target.wants
        ln -sf /etc/systemd/system/init_display.service ${D}/etc/systemd/system/multi-user.target.wants/init_display.service
    else
        install -d ${D}/${sysconfdir}/init.d
        install -m755 ${S}/init_qti ${D}/${sysconfdir}/init.d/weston
    fi
}
