inherit autotools pkgconfig qlicense qprebuilt

DESCRIPTION = "VAMEngines"

PR = "r0"

DEPENDS = "vam-lib vam-test scve-noship media"

CPPFLAGS += "-I${WORKSPACE}/frameworks/native/include/ui"
CPPFLAGS += "-I${STAGING_INCDIR}/adreno"
CPPFLAGS += "-I${STAGING_INCDIR}/opengl/include"
CPPFLAGS += "-I${STAGING_INCDIR}/fastcv"

TARGET_CFLAGS += "-I${STAGING_INCDIR}/qcom/display"

FILESPATH =+ "${WORKSPACE}/vendor/qcom/proprietary/:"
SRC_URI   = "file://vam-engines"
S = "${WORKDIR}/vam-engines"

SRC_DIR = "${WORKSPACE}/vendor/qcom/proprietary/vam-engines"

EXTRA_OECONF += " --with-basemachine=${BASEMACHINE}"
EXTRA_OECONF += " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF += "${@bb.utils.contains('DISTRO_FEATURES','face-recognition', ' --with-face-recognition ', '', d)}"

PACKAGES =+ "${PN}-ruleinterpreter-dbg ${PN}-ruleinterpreter ${PN}-ruleinterpreter-dev"
FILES_${PN}-ruleinterpreter-dbg    = "${libdir}/VAM/VAMEngines/.debug/libEngine_RuleInterpreter.*"
FILES_${PN}-ruleinterpreter        = "${libdir}/VAM/VAMEngines/libEngine_RuleInterpreter.so.*"
FILES_${PN}-ruleinterpreter-dev    = "${libdir}/VAM/VAMEngines/libEngine_RuleInterpreter.so ${libdir}/VAM/VAMEngines/libEngine_RuleInterpreter.la ${includedir}"


PACKAGES =+ "${PN}-facerecognition-dbg ${PN}-facerecognition ${PN}-facerecognition-dev"
FILES_${PN}-facerecognition-dbg    = "${libdir}/VAM/VAMEngines/.debug/libEngine_FaceRecognition.*"
FILES_${PN}-facerecognition        = "${libdir}/VAM/VAMEngines/libEngine_FaceRecognition.so.*"
FILES_${PN}-facerecognition-dev    = "${libdir}/VAM/VAMEngines/libEngine_FaceRecognition.so ${libdir}/VAM/VAMEngines/libEngine_FaceRecognition.la ${includedir}"


PACKAGES =+ "${PN}-motiontracker-dbg ${PN}-motiontracker ${PN}-motiontracker-dev"
FILES_${PN}-motiontracker-dbg    = "${libdir}/VAM/VAMEngines/.debug/libEngine_MotionTracker.*"
FILES_${PN}-motiontracker        = "${libdir}/VAM/VAMEngines/libEngine_MotionTracker.so.*"
FILES_${PN}-motiontracker-dev    = "${libdir}/VAM/VAMEngines/libEngine_MotionTracker.so ${libdir}/VAM/VAMEngines/libEngine_MotionTracker.la ${includedir}"

FILES_${PN}-qmmfalgpolaris-dbg    = "${libdir}/VAM/VAMEngines/.debug/libqmmf_alg_polaris_stitch.*"
FILES_${PN}-qmmfalgpolaris        = "${libdir}/VAM/VAMEngines/libqmmf_alg_polaris_stitch.so.*"
FILES_${PN}-qmmfalgpolaris-dev    = "${libdir}/VAM/VAMEngines/libqmmf_alg_polaris_stitch.so ${libdir}/VAM/VAMEngines/libqmmf_alg_polaris_stitch.la ${includedir}"

FILES_${PN}-qmmfalgsidebyside-dbg    = "${libdir}/VAM/VAMEngines/.debug/libqmmf_alg_side_by_side.*"
FILES_${PN}-qmmfalgsidebyside        = "${libdir}/VAM/VAMEngines/libqmmf_alg_side_by_side.so.*"
FILES_${PN}-qmmfalgsidebyside-dev    = "${libdir}/VAM/VAMEngines/libqmmf_alg_side_by_side.so ${libdir}/VAM/VAMEngines/libqmmf_alg_side_by_side.la ${includedir}"

SOLIBS = ".so*"
FILES_SOLIBSDEV = ""

do_install_append_apq8053() {
       install -d ${D}${libdir}/vam_engines/
       if [ "${@bb.utils.contains('DISTRO_FEATURES','face-recognition', 'true', 'false', d)}" == "true" ]; then
       install -c -m 0755 ${D}${libdir}/libEngine_FaceRecognition.so.0 -D ${D}${libdir}/vam_engines
       fi
       install -c -m 0755 ${D}${libdir}/libEngine_MotionTracker.so.0 -D ${D}${libdir}/vam_engines
       install -c -m 0755 ${D}${libdir}/libEngine_RuleInterpreter.so.0 -D ${D}${libdir}/vam_engines

       install -d ${D}${libdir}
       oe_libinstall -so -C ${S}/qmmf-alg/src/polaris/prebuilt/le/lib libpolaris ${D}${libdir}

       install -d ${D}/system/etc/qmmf/
       install -m 0700 ${S}/qmmf-alg/src/polaris/prebuilt/qmmf_polaris_calib.bin -D ${D}/system/etc/qmmf/
}

FILES_${PN} += "${libdir}/vam_engines/libEngine_FaceRecognition.so.0"
FILES_${PN} += "${libdir}/vam_engines/libEngine_MotionTracker.so.0"
FILES_${PN} += "${libdir}/vam_engines/libEngine_RuleInterpreter.so.0"

FILES_${PN} += "/system/etc/qmmf/qmmf_polaris_calib.bin"

INSANE_SKIP_${PN} = "dev-so"
do_configure[depends] += "virtual/kernel:do_shared_workdir"
