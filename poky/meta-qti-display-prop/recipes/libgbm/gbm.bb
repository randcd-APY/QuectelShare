inherit autotools qcommon qlicense
inherit autotools pkgconfig

DESCRIPTION = "libgbm Library"
PR = "r2"

SRC_DIR = "${WORKSPACE}/display/libgbm"
COLOR_METADATA_DIR = "${WORKSPACE}/display/display-hal"
S = "${WORKDIR}/display/libgbm/"

DEPENDS += "virtual/kernel wayland glib-2.0"

CFLAGS += "-I${PKG_CONFIG_SYSROOT_DIR}/usr/include/glib-2.0/"
CFLAGS += "-I${PKG_CONFIG_SYSROOT_DIR}/usr/lib64/glib-2.0/include"
CFLAGS += "-I${PKG_CONFIG_SYSROOT_DIR}/usr/lib/glib-2.0/include"
CFLAGS += "-DUSE_GLIB"

EXTRA_OECONF_append_apq8098 = " --enable-compilewithdrm"
EXTRA_OECONF_append_qcs605 = " --enable-compilewithdrm"
EXTRA_OECONF_append_sdmsteppe = " --enable-compilewithdrm"

LDFLAGS += "-lglib-2.0"

EXTRA_OECONF += " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
INSANE_SKIP_gbm += "dev-deps"
do_install_append () {
  install -d                                               ${D}${includedir}
  cp -rf ${S}/inc/gbm.h                                    ${D}${includedir}
  cp -rf ${S}/inc/gbm_priv.h                               ${D}${includedir}
  cp -rf ${COLOR_METADATA_DIR}/include/color_metadata.h    ${D}${includedir}
}
PACKAGES = "${PN}-dbg ${PN}"
FILES_${PN}-dbg  = "${libdir}/.debug/* ${bindir}/.debug/* /usr/lib/.debug/*"
FILES_${PN}      = "${libdir}/* /usr/lib/* ${bindir}/* ${includedir}/*"
do_configure[depends] += "virtual/kernel:do_shared_workdir"
