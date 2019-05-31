inherit autotools qcommon qlicense qprebuilt

DESCRIPTION = "libsdmextension Library"
PR = "r3"

PACKAGES = "${PN}"

SRC_DIR = "${WORKSPACE}/display/display-noship/"
S = "${WORKDIR}/display/display-noship/"

PREBUILT = "1"

DEPENDS += "display-hal-linux"
DEPENDS += "glib-2.0"
DEPENDS += "drm"
DEPENDS += "libdrm"

EXTRA_OECONF = " --with-core-includes=${WORKSPACE}/system/core/include"
EXTRA_OECONF += " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"

EXTRA_OECONF_append_apq8098 = " --enable-sdedrm"
EXTRA_OECONF_append_qcs605 = " --enable-sdedrm"
EXTRA_OECONF_append_sdmsteppe = " --enable-sdedrm"

LDFLAGS += "-llog -lutils -lcutils -lsdmutils -ldrm -lglib-2.0"
CPPFLAGS += "-DTARGET_HEADLESS"

CPPFLAGS += "-I${STAGING_INCDIR}/libdrm"
CPPFLAGS += "-I${WORKSPACE}/display/display-hal/include"
CPPFLAGS += "-I${WORKSPACE}/display/display-noship/hdr_tm"
CPPFLAGS += "-I${PKG_CONFIG_SYSROOT_DIR}/usr/include/glib-2.0"
CPPFLAGS += "-I${PKG_CONFIG_SYSROOT_DIR}/usr/lib64/glib-2.0/include"
CPPFLAGS += "-I${PKG_CONFIG_SYSROOT_DIR}/usr/lib/glib-2.0/include"
CPPFLAGS += "-DUSE_GLIB"

# Need the display sdm headers
CPPFLAGS += "-I${STAGING_INCDIR}/sdm"

FILES_${PN} = "${libdir}/*.so"
do_configure[depends] += "virtual/kernel:do_shared_workdir"
