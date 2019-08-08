inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel Alpha system"
PR = "r7"


SRC_DIR = "${WORKSPACE}/quectel-core/ql-mcm-at/"

S = "${WORKDIR}/quectel-core/ql-mcm-at"

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
    install -m 0644 ${S}/mcmat.service.in ${D}${systemd_unitdir}/system/mcmat.service

    install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/
    ln -sf ${systemd_unitdir}/system/mcmat.service \
    ${D}${systemd_unitdir}/system/multi-user.target.wants/mcmat.service
}

FILES_${PN} += "/lib/systemd/*"
FILES_${PN} += "/etc/*"
