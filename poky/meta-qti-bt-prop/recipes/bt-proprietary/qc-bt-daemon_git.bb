inherit autotools qcommon qlicense qprebuilt
DESCRIPTION = "Bluetooth QC Daemon"
PR = "r0"
DEPENDS = "data glib-2.0 system-core"

EXTRA_OECONF = " \
               --with-glib \
               "

SRC_DIR = "${WORKSPACE}/bt-proprietary/qc-bt-daemon"

CPPFLAGS_append = " -DUSE_ANDROID_LOGGING "
LDFLAGS_append = " -llog "

S = "${WORKDIR}/bt-proprietary/qc-bt-daemon"
