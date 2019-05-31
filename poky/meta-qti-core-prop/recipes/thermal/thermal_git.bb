inherit autotools

DESCRIPTION = "Thermal Daemon"
SECTION = "base"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
${LICENSE};md5=92b1d0ceea78229551577d4284669bb8"

PR = "r3"

SRC_URI = "file://${WORKSPACE}/thermal"

S = "${WORKDIR}/thermal"
DEPENDS = "qmi-framework glib-2.0"

EXTRA_OECONF = " --with-glib"

INITSCRIPT_NAME = "thermald"
INITSCRIPT_PARAMS = "start 40 2 3 4 5 . stop 80 0 1 6 ."

inherit update-rc.d

do_install_append() {
       install -m 0755 ${WORKDIR}/thermal/start_thermald_le -D ${D}${sysconfdir}/init.d/thermald
       install -m 0755 ${WORKDIR}/thermal/thermald-8930.conf -D ${D}${sysconfdir}/thermald.conf
}
