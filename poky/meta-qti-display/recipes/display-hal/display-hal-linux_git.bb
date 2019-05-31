inherit autotools qcommon

DESCRIPTION = "display Library"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/\
${LICENSE};md5=3775480a712fc46a69647678acb234cb"

PR = "r8"

PACKAGES = "${PN}"

SRC_DIR = "${WORKSPACE}/display/display-hal/"
S = "${WORKDIR}/display/display-hal/"

DEPENDS += "system-core"
DEPENDS += "libhardware"
DEPENDS += "drm"
DEPENDS += "libdrm"
DEPENDS += "gbm"
DEPENDS_remove_quec-smart += "gbm"
DEPENDS += "adreno"
DEPENDS_append_quec-smart += "libui binder"

EXTRA_OECONF = " --with-core-includes=${WORKSPACE}/system/core/include"
EXTRA_OECONF += " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"

EXTRA_OECONF_append_apq8098 = " --enable-sdmhaldrm"
EXTRA_OECONF_append_qcs605 = " --enable-sdmhaldrm"
EXTRA_OECONF_append_qcs40x = " --enable-sdmhalfb"
EXTRA_OECONF_append_sdmsteppe = " --enable-sdmhaldrm"

LDFLAGS += "-llog -lhardware -lutils -lcutils"

CPPFLAGS_append_apq8098 += "-DCOMPILE_DRM"
CPPFLAGS_append_qcs605 += "-DCOMPILE_DRM"
CPPFLAGS_append_sdmsteppe += "-DCOMPILE_DRM"

CFLAGS += "-I${STAGING_KERNEL_BUILDDIR}/usr/include"
CPPFLAGS += "-I${STAGING_KERNEL_BUILDDIR}/usr/include"
CPPFLAGS += "-DTARGET_HEADLESS"
CPPFLAGS += "-DVENUS_COLOR_FORMAT"
CPPFLAGS += "-DPAGE_SIZE=4096"
CPPFLAGS += "-D__GBM__"
CPPFLAGS_append_apq8098 += "-I${WORKSPACE}/display/display-hal/libdrmutils"
CPPFLAGS_append_qcs605 += "-I${WORKSPACE}/display/display-hal/libdrmutils"
CPPFLAGS_append_sdmsteppe += "-I${WORKSPACE}/display/display-hal/libdrmutils"

CPPFLAGS += "-I${WORKSPACE}/display/display-hal/gpu_tonemapper"
CPPFLAGS += "-I${WORKSPACE}/display/display-hal/sdm/include"
CPPFLAGS += "-I${WORKSPACE}/display/display-hal/include"
CPPFLAGS += "-I${WORKSPACE}/system/core/include"
CPPFLAGS_append_quec-smart += "-I${WORKSPACE}/display/display-hal/libqservice"
CPPFLAGS_append_quec-smart += "-I${WORKSPACE}/display/display-hal/libgralloc"
CPPFLAGS_append_quec-smart += "-I${WORKSPACE}/display/display-hal/libqdutils"
CPPFLAGS_append_apq8098 += "-I${STAGING_INCDIR}/libdrm"
CPPFLAGS_append_apq8098 += "-I${STAGING_INCDIR}/gbm"
CPPFLAGS_append_qcs605 += "-I${STAGING_INCDIR}/libdrm"
CPPFLAGS_append_qcs605 += "-I${STAGING_INCDIR}/gbm"
CPPFLAGS_append_sdmsteppe += "-I${STAGING_INCDIR}/libdrm"
CPPFLAGS_append_sdmsteppe += "-I${STAGING_INCDIR}/gbm"
CPPFLAGS_append_apq8098 += "-I${STAGING_INCDIR}/adreno"

do_configure[depends] += "virtual/kernel:do_shared_workdir"

do_install_append () {
    # libhardware expects to find /usr/lib/hw/gralloc.*.so
    install -d ${D}${libdir}/hw
    ln -s ${libdir}/libgralloc.so ${D}${libdir}/hw/gralloc.default.so
    cp -fR ${WORKSPACE}/display/display-hal/include/* ${STAGING_INCDIR}/
    cp -fR ${WORKSPACE}/display/display-hal/gpu_tonemapper/*.h ${STAGING_INCDIR}
}

FILES_${PN} = "${libdir}/*.so"
FILES_${PN} += "${libdir}/hw/gralloc.default.so"
INSANE_SKIP_${PN} = "dev-so"
