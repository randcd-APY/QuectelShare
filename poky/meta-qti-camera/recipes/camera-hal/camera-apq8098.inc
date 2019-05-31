
EXTRA_OECONF += "TARGET_SUPPORT=${BASEMACHINE}"
EXTRA_OECONF += "TARGET_BOARD_PLATFORM=8998"
EXTRA_OECONF += "TARGET_USES_ION=true"
EXTRA_OECONF += "TARGET_USES_MEDIA_EXTENSION=false"
EXTRA_OECONF += "TARGET_TS_MAKEUP=false"
EXTRA_OECONF += "TARGET_USES_AOSP=false"
EXTRA_OECONF += "VENUS_SUPPORTED=true"
EXTRA_OECONF += "UBWC_PRESENT=true"
EXTRA_OECONF += "TARGET_SUPPORTS_DUAL_JPEG=false"
EXTRA_OECONF += "JPEG_USE_PIPELINE=true"
EXTRA_OECONF += "DRONE_TARGET=false"

do_install_append() {
   mkdir -p ${D}/usr/lib/hw

   # Move and rename libcamera.so files to hw/machine-specific names.
   cp ${D}/usr/lib/libcamera_default.so ${D}/usr/lib/hw/libcamera.so

   pushd ${D}/usr/lib/hw
   ln -s libcamera.so ./camera.msm8998.so
   popd
}
