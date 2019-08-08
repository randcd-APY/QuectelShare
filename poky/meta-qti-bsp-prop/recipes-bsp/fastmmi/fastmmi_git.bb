inherit autotools qcommon qlicense qprebuilt
DESCRIPTION = "fastmmi"
PR = "r0"

def get_depends(d):
    if d.getVar('MACHINE', True) == 'msm8909-robot':
        return "${MLPREFIX}rb-mm-camera"
    elif d.getVar('BASEMACHINE', True) == 'qcs605':
        return ""
    elif d.getVar('DISTRO', True) == 'quec-smart':
        return "${MLPREFIX}rb-mm-camera"
    elif d.getVar('DISTRO', True) == 'quec-smart-ros':
        return "${MLPREFIX}rb-mm-camera"
    elif d.getVar('DISTRO', True) == 'concam':
        return ""
    elif d.getVar('DISTRO', True) == 'qsap':
        return ""
    else:
        return "${MLPREFIX}mm-camera"

DEPENDS += "glib-2.0 diag libxml2"
DEPENDS_append_arm += "${@get_depends(d)}"

INSANE_SKIP_${PN} += "dev-deps"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://vendor/qcom/proprietary/fastmmi/"
SRC_URI += "file://ffbm_mmi.service \
            file://qti_system_daemon.service"

S = "${WORKDIR}/vendor/qcom/proprietary/fastmmi"
SRC_DIR = "${WORKSPACE}/vendor/qcom/proprietary/fastmmi/"

LDFLAGS_append_quec-smart += "-llog"
LDFLAGS_append_quec-smart-ros += "-llog"

EXTRA_OECONF += "--with-glib"
EXTRA_OECONF_append += "--enable-product=${PRODUCT}"
EXTRA_OECONF_append += "--enable-target=${BASEMACHINE}"

CFLAGS_append = " -I${STAGING_INCDIR}/diag \
    `pkg-config --cflags glib-2.0`"

CXXFLAGS_append = " -I${STAGING_INCDIR}/libxml2 -I${STAGING_LIBDIR}/glib-2.0/include -I${STAGING_INCDIR}/diag \
    `pkg-config --cflags glib-2.0`"

CXXFLAGS_append += "-I${STAGING_KERNEL_BUILDDIR}/usr/include/media \
                    -I${STAGING_KERNEL_BUILDDIR}/usr/include \
                    -I${STAGING_KERNEL_BUILDDIR}/usr \
                    `pkg-config --cflags glib-2.0`"

FILES_${PN}-dbg  = "${libdir}/.debug/* ${bindir}/.debug/*"
FILES_${PN} += "/data/FTM_AP"
FILES_${PN} += "/vendor/etc"
FILES_${PN} += "/system/bin"
FILES_${PN} += "${libdir}/*.so"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"

do_install_append() {
    dest=/etc/mmi
    install -m 0755 -d ${D}${dest}
    # create FTM_AP folder
    install -dm0755 ${D}/data/FTM_AP
	install -dm0755 ${D}/vendor/etc
    # Add mmi.xml file
    #install -m 0755 ${S}/res/config/le/mmi.xml -D ${D}${dest}/mmi.xml
    install -m 0755 ${S}/res/values/path_config_le.xml -D ${D}${dest}/path_config.xml
    install -d ${D}${dest}/layout
    install -m 0755 ${S}/res/layout/*.xml -D ${D}${dest}/layout
    install -m 0755 ${S}/res/values/* -D ${D}${dest}
    install -m 0755 ${S}/res/raw/* -D ${D}${dest}
	install -m 0755 ${S}/res/audio_config/ftm_test_config -D ${D}/vendor/etc/ftm_test_config
	install -m 0755 ${S}/res/audio_config/mm-audio-ftm -D ${D}/usr/bin/mm-audio-ftm
	install -m 0755 ${S}/res/sim_config/qmi_simple_ril_test -D ${D}/system/bin/qmi_simple_ril_test
	install -m 0755 ${S}/res/listview_config/mmi.cfg -D ${D}/data/FTM_AP/mmi.cfg
	install -m 0755 ${S}/res/listview_config/mmi-auto.cfg -D ${D}/data/FTM_AP/mmi-auto.cfg
    install -m 0755 ${S}/res/wifi_config/wpa_supplicant.conf -D ${D}/etc/mmi
	
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -d ${D}${systemd_unitdir}/system/
        install -m 0644 ${WORKDIR}/ffbm_mmi.service -D ${D}${systemd_unitdir}/system/ffbm_mmi.service
        install -m 0644 ${WORKDIR}/qti_system_daemon.service -D ${D}${systemd_unitdir}/system/qti_system_daemon.service
        install -d ${D}${systemd_unitdir}/system/ffbm.target.wants/
        install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/
        # enable the service for multi-user.target
        ln -sf ${systemd_unitdir}/system/qti_system_daemon.service \
            ${D}${systemd_unitdir}/system/multi-user.target.wants/qti_system_daemon.service
        # enable the service for ffbm.target
        ln -sf ${systemd_unitdir}/system/qti_system_daemon.service \
            ${D}${systemd_unitdir}/system/ffbm.target.wants/qti_system_daemon.service
        ln -sf ${systemd_unitdir}/system/ffbm_mmi.service \
            ${D}${systemd_unitdir}/system/ffbm.target.wants/ffbm_mmi.service
    else
        install -m 0755 ${S}/res/config/le/start_mmi -D ${D}${sysconfdir}/init.d/start_mmi
        install -m 0755 ${S}/qti-system-daemon/start_qti_system -D ${D}${sysconfdir}/init.d/start_qti_system
    fi
}

pkg_postinst_${PN} () {
    [ -n "$D" ] && OPT="-r $D" || OPT="-s"
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'false', 'true', d)}; then
            update-rc.d $OPT -f start_mmi remove
            update-rc.d $OPT start_mmi start 98 4 .
            update-rc.d $OPT -f start_qti_system remove
            update-rc.d $OPT start_qti_system start 45 2 3 4 5 .
    fi
}

FILES_${PN} += "${systemd_unitdir}/system/"
do_configure[depends] += "virtual/kernel:do_shared_workdir"
