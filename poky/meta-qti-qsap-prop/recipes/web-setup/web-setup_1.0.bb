SUMMARY = "Web based setup sevice"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

FILESPATH_prepend = "${WORKSPACE}/vendor/qcom/proprietary/web-setup:"
SRC_URI = "\
            file://web-setup/ \
          "

S = "${WORKDIR}/web-setup/"

RDEPENDS_${PN} += "lighttpd jq wifi-scan net-config lighttpd-module-rewrite qcmapctl"

inherit allarch systemd

SERVER_ROOT="/www/websetup/"

do_install() {
    if [ -e "${S}/cgi" ]; then
        install -d ${D}${SERVER_ROOT}/cgi
        install -m 0755 ${S}/cgi/config ${D}${SERVER_ROOT}/cgi/
        install -m 0644 ${S}/cgi/web-setup.inc ${D}${SERVER_ROOT}/cgi/
    else
        install -d ${D}${SERVER_ROOT}
        cp -r ${S}/www/* ${D}${SERVER_ROOT}
    fi

    install -d ${D}${sysconfdir}
    install -m 0644 ${S}/websetup.lighttpd.conf ${D}${sysconfdir}

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${S}/websetup.service ${D}${systemd_system_unitdir}

    install -d ${D}${sbindir}
    install -m 0755 ${S}/websetup_configure -D ${D}${sbindir}/websetup_configure

    install -d ${D}/etc/device_configure.d
    install -m 0755 ${S}/device_configure/200-websetup -D ${D}/etc/device_configure.d/
}

FILES_${PN} += " \
    ${SERVER_ROOT} \
    "

SYSTEMD_SERVICE_${PN} = "websetup.service"
