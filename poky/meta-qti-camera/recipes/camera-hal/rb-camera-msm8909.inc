EXTRA_OECONF += "TARGET_SUPPORT=${BASEMACHINE}"
EXTRA_OECONF += "TARGET_BOARD_PLATFORM=8909"
EXTRA_OECONF += "TARGET_USES_ION=true"
EXTRA_OECONF += "DRONE_TARGET=false"
EXTRA_OECONF += "TARGET_USES_MEDIA_EXTENSION=false"
EXTRA_OECONF += "TARGET_COMPILE_WITH_MSM_KERNEL=true"
EXTRA_OECONF += "TARGET_TS_MAKEUP=true"
EXTRA_OECONF += "TARGET_COMPILE_WITH_MSM_KERNEL=true"
EXTRA_OECONF += "TARGET_IS_HEADLESS=true"
EXTRA_OECONF += "TARGET_USES_AOSP=false"
EXTRA_OECONF += "VENUS_SUPPORTED=true"
EXTRA_OECONF += "UBWC_PRESENT=false"
EXTRA_OECONF += "TARGET_SUPPORTS_DUAL_JPEG=false"
EXTRA_OECONF += "JPEG_USE_PIPELINE=false"

do_install_append() {
   mkdir -p ${D}/usr/lib/hw

   # Move and rename libcamera.so files to hw/machine-specific names.
   cp ${D}/usr/lib/libqcamera2.so.0.0.0 ${D}/usr/lib/hw/libcamera.so

   pushd ${D}/usr/lib/hw
   ln -s libcamera.so ./camera.msm8909.so
   popd

   mkdir -p ${STAGING_INCDIR}/cameracommon
   cp -rf ${S}/*.h ${STAGING_INCDIR}/cameracommon
}
