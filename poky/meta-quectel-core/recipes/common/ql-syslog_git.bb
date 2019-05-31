inherit qcommon qprebuilt qlicense

DESCRIPTION = "Quectel log system"
PR = "r7"

SRC_DIR = "${WORKSPACE}/quectel-core/ql-syslog/"

S = "${WORKDIR}/quectel-core/ql-syslog"

INITSCRIPT_NAME = "start_qllog"
INITSCRIPT_PARAMS = "start 41 2 3 4 5 ."
 
inherit update-rc.d

FILES_${PN} += "${libdir}/"
FILES_SOLIBSDEV = ""

do_install(){
    install -d ${D}${bindir}
    install -m 0755 bin/* ${D}${bindir}

    install -m 0755 etc/init.d/start_qllog -D ${D}${sysconfdir}/init.d/start_qllog
    install -m 0755 etc/qllog.json.sample -D ${D}${sysconfdir}/qllog.json.sample

    install -d ${D}${libdir}
    install -m 0755 lib/* ${D}${libdir}
}

do_compile() {
}

do_package_qa() {
}
