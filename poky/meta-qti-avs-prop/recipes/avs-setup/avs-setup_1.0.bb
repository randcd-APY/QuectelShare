SUMMARY = "AVS"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

FILESPATH_prepend = "${WORKSPACE}/vendor/qcom/proprietary/web-setup:"
SRC_URI = "\
            file://avs-setup \
          "

S = "${WORKDIR}/avs-setup/"

RDEPENDS_${PN} += "jq device-config web-setup mdnsresponder"

inherit allarch systemd

SERVER_CGI_DIR="/www/websetup/cgi"

do_install() {
    install -d ${D}${SERVER_CGI_DIR}
    install -m 0755 ${S}/avs.cgi -D ${D}${SERVER_CGI_DIR}/avs

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${S}/mdns_avs.service ${D}${systemd_system_unitdir}
    install -m 0644 ${S}/mdns_avs_stop.service ${D}${systemd_system_unitdir}
    install -m 0644 ${S}/mdns_avs_stop.path ${D}${systemd_system_unitdir}

    install -d ${D}${bindir}
    install -m 0755 ${S}/mdns_avs -D ${D}${bindir}/mdns_avs

    install -d ${D}${datadir}
    install -m 0755 ${S}/mdns_avs_functions.sh -D ${D}${datadir}
}

FILES_${PN} += " \
    ${SERVER_CGI_DIR} \
    ${datadir} \
    "

SYSTEMD_SERVICE_${PN} = " \
    mdns_avs.service \
    mdns_avs_stop.service \
    mdns_avs_stop.path \
    "
