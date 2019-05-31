inherit qcommon update-rc.d qprebuilt qlicense

DESCRIPTION = "QTI Power Saving Mode Framework"
PR = "r0"
PV = "git-invalidgit"

DEPENDS = "common qmi qmi-framework time-services data"

EXTRA_OECONF = "--with-glib \
                --with-common-includes=${STAGING_INCDIR} \
                --enable-target=${BASEMACHINE}"

SRC_DIR = "${WORKSPACE}/mdm-ss-mgr/psm/"
S = "${WORKDIR}/mdm-ss-mgr/psm/"

LDFLAGS += "-lglib-2.0"

INITSCRIPT_NAME = "psmd"
INITSCRIPT_PARAMS = "start 31 S ."

do_install_append() {
    if [ -d "${SRC_DIR}" ]; then
        install -d ${D}${includedir}
        install -m 0555 ${S}/client/psm_client.h ${D}${includedir}
        install -m 0755 ${S}/server/start_psmd -D ${D}${sysconfdir}/init.d/psmd
    fi
}

