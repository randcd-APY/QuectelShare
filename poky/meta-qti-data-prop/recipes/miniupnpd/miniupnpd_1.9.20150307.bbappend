FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
DEPENDS = "data libnfnetlink"
SRC_URI += "\
    file://0002-qcmap-enabled.patch \
    file://001-Makefile.patch \
"

do_install_append() {
              rm -rf ${D}${sysconfdir}/miniupnpd/
              install -d ${D}${sysconfdir}/data/miniupnpd
              install -m 644 ${WORKDIR}/${PN}-${PV}/miniupnpd.conf ${D}${sysconfdir}/data/miniupnpd
}
FILES_${PN} += "${sysconfdir}/data/miniupnpd/miniupnpd.conf"
