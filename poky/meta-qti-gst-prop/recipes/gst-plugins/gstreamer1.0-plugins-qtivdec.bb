inherit autotools qcommon
require ${COREBASE}/meta/recipes-multimedia/gstreamer/gstreamer1.0-plugins.inc
require ${COREBASE}/meta-qti-gst/recipes/gstreamer/gstreamer-common-submodule.inc
DEPENDS += "virtual/kernel"
SUMMARY = "video decoder Plugin for the GStreamer"
HOMEPAGE = ""
BUGTRACKER = ""
SECTION = "multimedia"

LIC_FILES_CHKSUM = "file://COPYING;md5=fbe5909056f71568e483d945fcbfe67e"
LICENSE = "QTI-Proprietary"

FILESPATH =+ "${WORKSPACE}/vendor/qcom/proprietary:"
SRC_URI   =  "file://gst-media"
S         =  "${WORKDIR}/gst-media"

DEPENDS += "gstreamer1.0-plugins-base gstreamer1.0-plugins-bad "

DEPENDS += "secure-gst gbm securemsm"

RDEPENDS_${PN} =+ "secure-gst"

INSANE_SKIP_${PN} += "dev-deps"

PR = "r1"

inherit gettext

# For using v4l2dec gstreamer plugins with MSM_VIDC driver
# you MUST force the option "--without-libv4l"
PACKAGECONFIG ??= ""
PACKAGECONFIG[qtivdec] = "--with-libv4l2,--without-libv4l2,v4l-utils"
EXTRA_OECONF += " \
    ${GSTREAMER_1_0_ORC} \
"
FILES_${PN} += " ${libdir}/gstreamer-1.0/*.so "
FILES_${PN}-dbg += "${libdir}/gstreamer-1.0/.debug"
GSTREAMER_ORC ?= "orc"

PACKAGECONFIG[debug] = "--enable-debug,--disable-debug"
PACKAGECONFIG[orc] = "--enable-orc,--disable-orc,orc orc-native"

do_configure_prepend() {
    srcdir=${S} NOCONFIGURE=1 ${S}/autogen.sh
}
CFLAGS += "-I${STAGING_KERNEL_BUILDDIR}/usr/include"
CFLAGS += "-I${WORKSPACE}/vendor/qcom/proprietary/gst-media/secure"
CFLAGS += "-I${WORKSPACE}/security/securemsm/sampleclient"
CFLAGS += "-I${WORKSPACE}/display/libgbm/inc"

