inherit qcommon qlicense qprebuilt update-rc.d

DESCRIPTION = "QTI Logkit"
# Recipe version - increment any time this file changes
PR = "r1"

DEPENDS = "zlib"

# Set default directories
SRC_DIR = "${WORKSPACE}/vendor/qcom/proprietary/qti-logkit/"
S = "${WORKDIR}/vendor/qcom/proprietary/qti-logkit/"

INITSCRIPT_NAME = "startLKCore"
# Start and stop with default runlevels
# seq=20/80 so LKCore is started/stoped after/before diag
INITSCRIPT_PARAMS = "start 20 2 3 4 5 . stop 80 0 1 6 ."

# Add init script to file system
do_install_append() {
    install -D -m 0755 \
        ${WORKDIR}/vendor/qcom/proprietary/qti-logkit/LKCore/startLKCore \
        ${D}${sysconfdir}/init.d/startLKCore
}
