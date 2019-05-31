inherit autotools qcommon qlicense qprebuilt

DESCRIPTION = "Engine Plugin API hdr"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"
SRC_DIR = "${WORKSPACE}/gps/engine-plugin/api/"
S = "${WORKDIR}/gps/engine-plugin/api/"
FILES_${PN} += "/usr/*"
PACKAGES = "${PN}"

do_configure() {
}

do_compile() {
}

do_install() {
    install -d ${D}${includedir}
    install -m 644 ${S}/*.h ${D}${includedir}
}
