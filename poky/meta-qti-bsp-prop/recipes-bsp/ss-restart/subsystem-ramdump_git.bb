inherit autotools-brokensep pkgconfig update-rc.d qprebuilt
DESCRIPTION = "subsystem ramdump"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti-bsp-prop/files/qcom-licenses/\
${LICENSE};md5=92b1d0ceea78229551577d4284669bb8"
PR = "r3"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://ss-restart/subsystem_ramdump/"
SRC_DIR = "${WORKSPACE}/ss-restart/subsystem_ramdump/"

S = "${WORKDIR}/ss-restart/subsystem_ramdump/"

DEPENDS = "glib-2.0"

EXTRA_OECONF = " --with-glib"

INITSCRIPT_NAME = "start_subsystem_ramdump"
INITSCRIPT_PARAMS = "start 90 2 3 4 5 . stop 10 0 1 6 ."

do_install_append() {
       install -m 0755 ${S}/start_subsystem_ramdump -D ${D}${sysconfdir}/init.d/start_subsystem_ramdump
}
