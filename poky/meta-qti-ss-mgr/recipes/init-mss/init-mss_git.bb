inherit autotools-brokensep update-rc.d systemd

DESCRIPTION = "Modem init"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/BSD;md5=3775480a712fc46a69647678acb234cb"
PR = "r7"

FILESPATH =+ "${WORKSPACE}:"
FILESEXTRAPATHS_prepend := "${THISDIR}/init_mss:"

SRC_URI = "file://mdm-ss-mgr/init_mss/"
SRC_URI += "file://init_sys_mss.service"

S = "${WORKDIR}/mdm-ss-mgr/init_mss/"

# Hold /dev/subsys_modem forever on all SOCs which don't have Modem wakeup support.
EXTRA_OECONF_append_msm = " --enable-indefinite-sleep"
EXTRA_OECONF_append_sdxpoorwills = " --enable-indefinite-sleep"

EXTRA_OECONF_append = " --enable-modem"

# QCS40x has wcnss but not modem
EXTRA_OECONF_remove_qcs40x = "--enable-modem"
EXTRA_OECONF_append_qcs40x = " --enable-wcnss"

FILES_${PN} += "${systemd_unitdir}/system/"

INITSCRIPT_NAME = "init_sys_mss"
INITSCRIPT_PARAMS = "start 38 2 3 4 5 ."
INITSCRIPT_PARAMS_sdxpoorwills = "start 31 S ."

do_install() {
    install -m 0755 ${S}/init_mss -D ${D}/sbin/init_mss
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -d ${D}${systemd_unitdir}/system/
        install -d ${D}${systemd_unitdir}/system/sysinit.target.wants/
        install -m 0644 ${WORKDIR}/init_sys_mss.service -D ${D}${systemd_unitdir}/system/init_sys_mss.service
        ln -sf ${systemd_unitdir}/system/init_sys_mss.service ${D}/${systemd_unitdir}/system/sysinit.target.wants/init_sys_mss.service
    else
        install -m 0755 ${S}/start_mss -D ${D}${sysconfdir}/init.d/init_sys_mss
    fi
}
