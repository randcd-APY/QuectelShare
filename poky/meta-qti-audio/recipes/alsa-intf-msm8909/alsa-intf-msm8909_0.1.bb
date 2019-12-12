
DESCRIPTION = "ALSA Framework Library so"
LICENSE = "Apache-2.0"
PR = "r6"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/${LICENSE};md5=89aea4e17d99a7cacdbeed46a0096b10"
DEPENDS = "acdbloader glib-2.0"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://quectel-core/mm-audio/"

S = "${WORKDIR}/quectel-core/mm-audio/"

EXTRA_OECONF += "--prefix=/etc/msm8909 \
                 --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
                 --with-glib \
                 --with-acdb"

FILES_${PN} += "${prefix}/snd_soc_msm/*"
PACKAGES = "${PN}"
FILES_${PN} += "${libdir}/*"
FILES_${PN} += "${includedir}/*"
FILES_SOLIBSDEV = ""
INSANE_SKIP_${PN} = "dev-so"

BBCLASSEXTEND = "native nativesdk"


do_configure[depends] += "virtual/kernel:do_shared_workdir"

do_install() {
	install -d ${D}${libdir}
	install -m 0755 ${S}usr/lib/* ${D}${libdir}
	install -d ${D}${includedir}/alsa-intf-msm8909
	install -m 0644 ${WORKSPACE}/quectel-core/mm-audio/usr/include/alsa-intf-msm8909/*.h ${D}${includedir}/alsa-intf-msm8909/
	install -d ${D}/etc/msm8909/snd_soc_msm
	install -m 0644 ${WORKSPACE}/quectel-core/mm-audio/etc/msm8909/snd_soc_msm/snd_soc_msm_9x07_Tomtom_I2S ${D}/etc/msm8909/snd_soc_msm/
}