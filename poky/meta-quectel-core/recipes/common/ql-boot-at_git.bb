inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel Alpha system"
PR = "r7"


SRC_DIR = "${WORKSPACE}/quectel-core/ql-boot-at/"

S = "${WORKDIR}/quectel-core/ql-boot-at"

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
    install -m 0644 ${S}/bootat.service.in ${D}${systemd_unitdir}/system/bootat.service

    install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/
    ln -sf ${systemd_unitdir}/system/bootat.service \
    ${D}${systemd_unitdir}/system/multi-user.target.wants/bootat.service
}

FILES_${PN} += "/lib/systemd/*"
FILES_${PN} += "/etc/*"
