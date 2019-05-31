inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel Alpha system"
PR = "r7"


SRC_DIR = "${WORKSPACE}/quectel-core/ql-at/"

S = "${WORKDIR}/quectel-core/ql-at"

do_install(){
    install -d ${D}${bindir}
    install -m 0755 bin/* ${D}${bindir}    
}

do_compile() {
}

do_package_qa() {
}

do_install_append() {
    install -d ${D}${systemd_unitdir}/system
    install -m 0644 ${S}/at_network.service.in ${D}${systemd_unitdir}/system/at_network.service

    install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/
    ln -sf ${systemd_unitdir}/system/at_network.service \
    ${D}${systemd_unitdir}/system/multi-user.target.wants/at_network.service

    install -m 0755 ${S}/at_network_le -D ${D}${sysconfdir}/initscripts/at_network_le
}

FILES_${PN} += "/lib/systemd/*"
FILES_${PN} += "/etc/*"
