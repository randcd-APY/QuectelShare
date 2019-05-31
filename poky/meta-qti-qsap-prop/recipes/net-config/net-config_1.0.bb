SUMMARY = "Network Configuration"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

FILESPATH_prepend = "${WORKSPACE}/vendor/qcom/proprietary/web-setup:"
SRC_URI = "\
            file://net-config \
          "

S = "${WORKDIR}/net-config/"

RDEPENDS_${PN} += "wifi-scan device-config iw"

inherit allarch

do_install() {
    install -d ${D}${base_sbindir}
    install -m 0755 ${S}/wifi_try ${D}${base_sbindir}
}
