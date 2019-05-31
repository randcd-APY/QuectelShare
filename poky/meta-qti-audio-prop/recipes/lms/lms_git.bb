inherit qcommon qprebuilt qlicense

DESCRIPTION = "License Management System"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-noship/lms/"
S = "${WORKDIR}/audio/mm-audio-noship/lms/"

DEPENDS  = "glib-2.0"
EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF += "--with-glib"

do_configure[depends] += "virtual/kernel:do_shared_workdir"

do_install() {

    install -d ${D}${includedir}/lms
    install -m 0644 ${S}/lms/include/*.h ${D}${includedir}/lms

    install -d ${D}${libdir}
    install -m 0755 ${S}lms/.libs/liblms.so ${D}/${libdir}

}

SOLIBS = ".so"
FILES_SOLIBSDEV = ""

# Include these files in the install on the target
FILES_${PN} += "${libdir}/liblms.so"
