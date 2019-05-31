inherit qcommon qprebuilt qlicense

DESCRIPTION = "qvop-service Library and test app"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio/voiceprint/qvop-service/"
S = "${WORKDIR}/audio/mm-audio/voiceprint/qvop-service/"

DEPENDS = "audiohal securemsm-noship qsthw-api"

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
                 --with-glib"

CPPFLAGS += "-I${WORKSPACE}/security/securemsm/QSEEComAPI"

do_install_append() {
      install -d ${D}${sysconfdir}/qvop/
      install -m 0755 ${S}/calib/* -D ${D}${sysconfdir}/qvop/
}

INSANE_SKIP_${PN} = "dev-deps"
do_configure[depends] += "virtual/kernel:do_shared_workdir"
