inherit autotools qcommon qlicense qprebuilt systemd

SUMMARY = "Command line tool to control QCMAP"

DEPENDS = "data boost"

# Dependencies that should be inherited from data, but when using the
# prebuilt version, those dependencies are not correctly handled by
# bitbake
DEPENDS += "dsutils qmi qmi-framework common configdb xmllib diag time-genoff glib-2.0"

FILESPATH_prepend = "${WORKSPACE}/:"
SRC_URI += "file://vendor/qcom/proprietary/web-setup/qcmapctl/qcmapctl_le"

SRC_DIR = "${WORKSPACE}/vendor/qcom/proprietary/web-setup/qcmapctl/"
S = "${WORKDIR}/vendor/qcom/proprietary/web-setup/qcmapctl/"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/qcmapctl ${D}${bindir}

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${S}/qcmapctl.service -D ${D}${systemd_system_unitdir}

    install -m 0755 ${S}/qcmapctl_le -D ${D}${sysconfdir}/initscripts/qcmapctl_le
}

FILES_${PN} += " \
    ${systemd_system_unitdir} \
    ${sysconfdir}/* \
    "

SYSTEMD_SERVICE_${PN} = "qcmapctl.service"
