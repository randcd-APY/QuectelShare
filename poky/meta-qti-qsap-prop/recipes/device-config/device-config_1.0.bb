SUMMARY = "Device Configure"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

FILESPATH_prepend = "${WORKSPACE}/vendor/qcom/proprietary/web-setup:"
SRC_URI = "\
            file://device-config \
          "

S = "${WORKDIR}/device-config/"

RDEPENDS_${PN} += "jq"

inherit allarch systemd


do_install() {
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${S}/device_configure.service ${D}${systemd_system_unitdir}

    install -d ${D}${sbindir}
    install -m 0755 ${S}/device_configure -D ${D}${sbindir}/device_configure
}


SYSTEMD_SERVICE_${PN} = "device_configure.service"
