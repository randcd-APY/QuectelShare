SUMMARY = "AllPlay crash reporter"
LICENSE = "CLOSED"
LIC_FILES_CHKSUM = ""

PR="r1"

SRC_URI = " \
    file://submitCore.sh \
    file://crashreporter.service \
    "

RDEPENDS_${PN} += "ca-certificates"

inherit allarch systemd

do_install() {
    install -d ${D}${base_sbindir}
    install -m 0755 ${WORKDIR}/submitCore.sh ${D}${base_sbindir}

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/crashreporter.service -D ${D}${systemd_system_unitdir}
}

FILES_${PN} += " \
    ${systemd_system_unitdir} \
    "

SYSTEMD_SERVICE_${PN} = "crashreporter.service"
