inherit autotools pkgconfig qlicense qprebuilt

DESCRIPTION = "MM Camera libraries for MSM/QSD"
SECTION  = "camera"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI   = "file://camera/services/mm-camera-lib-legacy/"

SRCREV = "${AUTOREV}"
S      = "${WORKDIR}/camera/services/mm-camera-lib-legacy/"

SRC_DIR   = "${WORKSPACE}/camera/services/mm-camera-lib-legacy/"

DEPENDS += "glib-2.0 libxml2"
DEPENDS += "fastcv-noship"
DEPENDS_append_quec-smart += "libcutils liblog adsprpc"
DEPENDS_remove_quec-smart += "fastcv-noship"

do_package_qa[noexec] = "1"

EXTRA_OECONF = "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include \
                --with-glib \
                --with-common-includes=${STAGING_INCDIR} \
                --with-camera-hal-path=${WORKSPACE}/camera/lib-legacy \
                --with-mm-camera-path=${WORKSPACE}/camera/services/mm-camera-legacy/mm-camera2 \
                --with-camera-core=${WORKSPACE}/camera/services/mm-camera-core-legacy \
                --with-3a-core=${WORKSPACE}/camera/services/mm-3a-core-legacy \
                --with-mm-camerasdk=${WORKSPACE}/camera/services/mm-camerasdk"

EXTRA_OECONF_append_msm8909 += "TARGET_BOARD_PLATFORM=8909"
EXTRA_OECONF += "CHROMATIX_VERSION=0301"
EXTRA_OECONF += "COMPILE_MMCAMERA2=true"
EXTRA_OECONF += "USE_C_FUSION=true"
EXTRA_OECONF += "TARGET_ARM32=true"
EXTRA_OECONF += "GPU_USE_NEW_OPENCL_API=true"
EXTRA_OECONF += "WAVELET_DENOISE_PATH=wavelet_denoise"
EXTRA_OECONF += "TARGET_NEON_ENABLED=true"
EXTRA_OECONF += "TARGET_DSP_ENABLED=true"
EXTRA_OECONF += "TARGET_GPU_ENABLED=false"
EXTRA_OECONF += "CACGPU_DEBUG_LOGS_ENABLED=false"
EXTRA_OECONF += "CACGPU_GPU_ENABLED=true"
EXTRA_OECONF_append_quec-smart += "_CP_USE_FAST_CV_=false"
EXTRA_OECONF_append_quec-smart += "USE_FASTCV_OPT=false"

#Below CFLAGS can be moved to Makefiles

do_install_append() {
   mkdir ${D}/usr/bin/
   cp ${S}cp/prebuilt/*.so ${D}${libdir}/
   cp ${S}llvd/prebuilt/llvd_sm/lib/*.so ${D}${libdir}/
   install -d ${D}/usr
   install -d ${D}/usr/lib
   install -d ${D}/usr/include
   install -d ${D}/usr/include/mm-camera
   install  ${S}/tintless/be/dmlrocorrection.h ${D}/usr/include/mm-camera
   install  ${S}/tintless/bg/pca_rolloff/dmlrocorrection_bg_pca.h ${D}/usr/include/mm-camera
   install -d ${D}/usr/lib
   mkdir -p ${D}/system && mv ${D}/usr/bin/ ${D}/system
}

PACKAGES = "${PN}"
#FILES_${PN}      += "${libdir}/ /usr/lib/* ${includedir} /system/etc/* "
FILES_${PN} +=  "/usr/lib/* \
                 /usr/include/*"
FILES_${PN}-dbg  += "${libdir}/.debug/* ${includedir} "
INSANE_SKIP_${PN} = "dev-so"
INSANE_SKIP_${PN} += "arch"
INSANE_SKIP_${PN} += "installed-vs-shipped"
INSANE_SKIP_${PN} += "textrel"
INSANE_SKIP_${PN} += "already-stripped"
INSANE_SKIP_${PN} += "ldflags"
INSANE_SKIP_${PN} += "staticdev"
