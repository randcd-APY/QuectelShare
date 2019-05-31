inherit qcommon qprebuilt qlicense

DESCRIPTION = "Audio FTM"
PR = "r5"

SRC_DIR = "${WORKSPACE}/audio/mm-audio/audio_ftm/"
S = "${WORKDIR}/audio/mm-audio/audio_ftm/"

DEPENDS = "common tinyalsa glib-2.0 diag"
DEPENDS_append = "${@bb.utils.contains('DISTRO_FEATURES', 'audio-dlkm', '', '', d)}"
DEPENDS_append_mdm = " alsa-intf"

do_configure[depends] += "virtual/kernel:do_shared_workdir"

EXTRA_OECONF_append = " --with-common-includes=${STAGING_INCDIR}"
EXTRA_OECONF_append = " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF_append_sdxpoorwills = " --with-sanitized-headers-audio=${STAGING_KERNEL_BUILDDIR}/usr/techpack/audio/include"
EXTRA_OECONF_append_mdm = " --with-sanitized-headers-audio=${STAGING_KERNEL_BUILDDIR}/usr/techpack/audio/include"
EXTRA_OECONF_append = " --with-glib"
EXTRA_OECONF_append_mdm = " IS_LEGACY=true"

do_install_append_mdm9607() {
    mkdir -p ${D}${sysconfdir}/
    install -m 0755 ${S}/legacy_intf/config/9607/ftm_test_config*  -D ${D}${sysconfdir}
}

do_install_append_apq8053() {
    mkdir -p ${D}${sysconfdir}/
    install -m 0755 ${S}/family-b/config/8953/ftm_test_config  -D ${D}${sysconfdir}
}

do_install_append_apq8017() {
    mkdir -p ${D}${sysconfdir}/
    install -m 0755 ${S}/family-b/config/8937/ftm_test_config_wcd9335  -D ${D}${sysconfdir}
    cd ${D}/${sysconfdir}/ && mv ftm_test_config_wcd9335 ftm_test_config_msm8952-tasha-snd-card
}

do_install_append_msm8909() {
    mkdir -p ${D}${sysconfdir}/
    install -m 0755 ${S}/family-b/config/8909/ftm_test_config  -D ${D}${sysconfdir}
}

do_install_append_apq8096() {
    mkdir -p ${D}${sysconfdir}/
    install -m 0755 ${S}/family-b/config/8996/ftm_test_config  -D ${D}${sysconfdir}
}

FILES_${PN}-dbg  = "${libdir}/.debug/* ${bindir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${bindir}/* ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
