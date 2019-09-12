inherit qcommon qlicense qprebuilt

DESCRIPTION = "mcm headers"
PR = "r3"

DEPENDS = ""

EXTRA_OECONF = "--with-common-includes=${STAGING_INCDIR}"

SRC_DIR = "${WORKSPACE}/mcm-api/"
S       = "${WORKDIR}/mcm-api/"

do_compile[noexec] = "1"

do_install () {
    if [ -d "${SRC_DIR}" ]; then
        install -d ${D}${includedir}
        install -m 644 ${S}/api/*.h ${D}${includedir}
    else
        qprebuilt_do_install
    fi
}

PACKAGES = "${PN}"
#FILES_${PN} += "${libdir}/*"
FILES_${PN} += "${includedir}/*"
INSANE_SKIP_${PN} = "dev-so"

BBCLASSEXTEND = "native nativesdk"
