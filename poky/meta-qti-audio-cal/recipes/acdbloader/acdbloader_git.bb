inherit autotools qcommon qprebuilt qlicense

DESCRIPTION = "acdb loader Library"
PR = "r7"

SRC_DIR = "${WORKSPACE}/audio/mm-audio-cal/audio-acdb-util/acdb-loader/"
S = "${WORKDIR}/audio/mm-audio-cal/audio-acdb-util/acdb-loader"

DEPENDS = "glib-2.0 audcal acdbrtac adiertac libcutils system-core"
DEPENDS_append = "${@bb.utils.contains('DISTRO_FEATURES', 'audio-dlkm', '', '', d)}"

do_configure[depends] += "virtual/kernel:do_shared_workdir"

do_install_append(){
    install -d ${D}${sysconfdir}/firmware/wcd9310
    ln -sf /data/misc/audio/wcd9310_anc.bin  ${D}${sysconfdir}/firmware/wcd9310/wcd9310_anc.bin
    ln -sf /data/misc/audio/mbhc.bin  ${D}${sysconfdir}/firmware/wcd9310/wcd9310_mbhc.bin
}

EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
                 --with-sanitized-headers-audio=${STAGING_KERNEL_BUILDDIR}/usr/techpack/audio/include \
                 --with-glib \
                 --enable-target=${BASEMACHINE}"

EXTRA_OEMAKE_apq8053 = "AM_CFLAGS="-D AUDIO_USE_SYSTEM_HEAP_ID""
EXTRA_OEMAKE_apq8017 = "AM_CFLAGS="-D AUDIO_USE_SYSTEM_HEAP_ID""
EXTRA_OEMAKE_apq8098 = "AM_CFLAGS="-D AUDIO_USE_SYSTEM_HEAP_ID""
EXTRA_OEMAKE_apq8096 = "AM_CFLAGS="-D AUDIO_USE_SYSTEM_HEAP_ID""
EXTRA_OEMAKE_qcs605  = "AM_CFLAGS="-D AUDIO_USE_SYSTEM_HEAP_ID""

SOLIBS = ".so"
FILES_SOLIBSDEV = ""
