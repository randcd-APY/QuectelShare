inherit autotools qlicense 

DESCRIPTION = "OpenMAX video for MSM chipsets"

PR = "r9"

SRC_URI = "file://${WORKSPACE}/mm-video-prop"
S = "${WORKDIR}/mm-video-prop"

DEPENDS = "mm-core-oss"
DEPENDS += "mm-video-oss"

EXTRA_OECONF_append = " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include "
EXTRA_OECONF_append = " --enable-target=${BASEMACHINE}"

CPPFLAGS = "-I${STAGING_INCDIR}/glib-2.0"
CPPFLAGS += "-I${STAGING_LIBDIR}/glib-2.0/include"

CPPFLAGS += "-I${STAGING_INCDIR}/c++"
CPPFLAGS += "-I${STAGING_INCDIR}/c++/${TARGET_SYS}"

LDFLAGS = "-lglib-2.0 -Wl,--build-id=sha1"

#Skips check for .so symlinks
INSANE_SKIP_${PN} = "dev-so"

PACKAGES = "${PN}"
#Disable the split of debug information into -dbg files
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"

FILES_${PN} = "/lib/firmware/*"

#Don't try stripping binaries
INHIBIT_PACKAGE_STRIP = "1"

do_install_append_msm8655() {
   mkdir -p ${D}/lib/firmware
   cp -pPr ${S}/vidc/firmware-720p/*.fw ${D}/lib/firmware
}
do_install_append_msm8960() {
   mkdir -p ${D}/lib/firmware
   cp -pPr ${S}/vidc/firmware-1080p/*.fw ${D}/lib/firmware
}
do_install_append_msm8974() {
   mkdir -p ${D}/lib/firmware
   cp -pPr ${S}/vidc/firmware-msm8974/venus.* ${D}/lib/firmware
}
do_install_append_msm8226() {
   mkdir -p ${D}/lib/firmware
   cp -pPr ${S}/vidc/firmware-msm8226/venus.* ${D}/lib/firmware
}
do_configure[depends] += "virtual/kernel:do_shared_workdir"
