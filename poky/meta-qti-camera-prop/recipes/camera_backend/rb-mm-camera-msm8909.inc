EXTRA_OECONF += "DRONE_TARGET=false"
EXTRA_OECONF += "TARGET_BOARD_PLATFORM=msm8909"
EXTRA_OECONF += "USE_CAMERA_STUB=true"
EXTRA_OECONF += "TARGET_USES_ION=true"
EXTRA_OECONF += "FEATURE_GYRO=false"
EXTRA_OECONF += "DISABLE_GYRO_MODULE=true"
EXTRA_OECONF += "DRONE_TARGET=${DRONE_TARGET}"
EXTRA_OECONF += "TARGET_USES_MEDIA_EXTENSION=false"
EXTRA_OECONF += "TARGET_COMPILE_WITH_MSM_KERNEL=true"
EXTRA_OECONF += "TARGET_TS_MAKEUP=true"
EXTRA_OECONF += "USE_GYRO=false"
EXTRA_OECONF += "IS_ENABLE=true"
EXTRA_OECONF += "CHROMATIX_VERSION=0301"
EXTRA_OECONF += "VPE_ENABLE=false"
EXTRA_OECONF += "VPU_ENABLE=false"
EXTRA_OECONF += "FEATURE_SVHDR_EN=true"
EXTRA_OECONF += "FEATURE_BINCORR=true"
EXTRA_OECONF += "FEATURE_CAC=true"
EXTRA_OECONF += "FEATURE_SW_TNR=true"
EXTRA_OECONF += "ENABLED_IMX241_RAW10_MODES=false"
EXTRA_OECONF += "MM_DEBUG=true"
EXTRA_OECONF += "VFE_VERS=xvfe32"
EXTRA_OECONF += "TARGET_NEON_ENABLED=false"
EXTRA_OECONF += "FEATURE_ZSL=true"
EXTRA_OECONF += "FEATURE_FACE_PROC=true"
EXTRA_OECONF += "FEATURE_VFE_TEST_VEC=false"
EXTRA_OECONF += "BUILD_SECURE_APP=true"
EXTRA_OECONF += "JPEG_DMA=false"
EXTRA_OECONF += "FD_USES_DSP=true"
EXTRA_OECONF += "USE_CDSP=false"
EXTRA_OECONF += "FD_ENABLE_SW_INTERMITTENT=false"
EXTRA_OECONF += "FD_USE_V65=true"
EXTRA_OECONF += "IMGLIB_USES_HVX=false"
EXTRA_OECONF += "IMGLIB_USES_FASTCV=false"
EXTRA_OECONF += "FACEPROC_USE_NEW_WRAPPER=true"
EXTRA_OECONF += "USE_CAC_V1=false"
EXTRA_OECONF += "USE_CAC_V2=false"
EXTRA_OECONF += "USE_CAC_V3=true"
EXTRA_OECONF += "PROFILE_FD=true"
EXTRA_OECONF += "USE_FASTCV_OPT=false"
EXTRA_OECONF += "DISABLE_FASTCV=true"
EXTRA_OECONF += "BOARD_QTI_CAMERA_V2=false"
EXTRA_OECONF += "CAMERA_USE_CHROMATIX_HW_WNR_TYPE=true"
EXTRA_OECONF += "LOCAL_MACHINE=msm8909"
EXTRA_OECONF += "TARGET_LIBRARY_SUPPRESS_LIST=libcamera_client libhardware libadsprpc \
        libsync libui libcamera_metadata libqdMetaData libqservice libbinder libgui libstlport libandroid libxml2 libz"

do_install_append() {
   cp ${S}/mm-camera2/media-controller/modules/imglib/components/lib/faceproc/libmmcamera_faceproc.so ${D}${libdir}/libmmcamera_faceproc.so
   install -d ${D}/data/misc/camera

   install -d ${D}${systemd_unitdir}/system/
   install -m 0644 ${WORKDIR}/mm-qcamera-daemon.service -D ${D}${systemd_unitdir}/system/mm-qcamera-daemon.service
   install -d ${D}${systemd_unitdir}/system/multi-user.target.wants/
   install -d ${D}${systemd_unitdir}/system/ffbm.target.wants/
   # enable the service for multi-user.target
   ln -sf ${systemd_unitdir}/system/mm-qcamera-daemon.service \
          ${D}${systemd_unitdir}/system/multi-user.target.wants/mm-qcamera-daemon.service
   # enable the service for ffbm.target
   ln -sf ${systemd_unitdir}/system/mm-qcamera-daemon.service \
          ${D}${systemd_unitdir}/system/ffbm.target.wants/mm-qcamera-daemon.service
   #cp ${BASE_WORKDIR}/${MULTIMACH_TARGET_SYS}/rb-mm-still/0.1-r0/image/${bindir}/* ${D}/system/bin/

   mkdir -p ${D}/system/etc/
   cp -r ${D}/lib/firmware/ ${D}/system/etc/

  # install -m 0644 ${WORKDIR}/mm-qcamera.conf -D ${D}${sysconfdir}/init/mm-qcamera.conf
}
