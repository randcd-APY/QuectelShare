inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel Common Tools"
PR = "r7"

DEPENDS = "adns"

S       =  "${WORKDIR}/quectel-core/ql-common-tools"
SRC_DIR =  "${WORKSPACE}/quectel-core/ql-common-tools/"

FILES_${PN} += "${libdir}/" 

FILES_SOLIBSDEV = ""


do_install() {
    install -d ${D}${libdir}
    #install -m 0755 lib/* ${D}${libdir}
    cp -a lib/* ${D}${libdir}
}

do_compile() {
}
