inherit qcommon qlicense qprebuilt qsigning

DESCRIPTION = "ms12"
SECTION = "multimedia"
PR = "r0"

MS12BINSUFFIX = "${@base_contains('TUNE_ARCH', 'aarch64', '_64bit', '', d)}"
DEPENDS = "glib-2.0 libcutils system-media acdbloader tinycompress tinyalsa audio-qaf gensecimage audio-qap-wrapper audio-dolby-ms12-external-noship python-native"

EXTRA_OEMAKE = "DEFAULT_INCLUDES= CPPFLAGS="-I. -I${STAGING_KERNEL_BUILDDIR}/usr/include""

SRC_DIR = "${WORKSPACE}/audio/mm-audio-noship/audio-codecs/ms12/"
S = "${WORKDIR}/audio/mm-audio-noship/audio-codecs/ms12/"
SECIMAGE_BASE = "${SIGNING_TOOLS_DIR}/SecImage"
PROD_CONFIG_DIR = "${WORKSPACE}/security/securemsm-internal/Sectools_Prod_UIE"

EXTRA_OECONF = "--with-glib --program-suffix=${MS12BINSUFFIX}"
EXTRA_OECONF += "--with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"
EXTRA_OECONF_append_apq8098 = " MS12_SECURITY_FEATURE_ENABLED=true"
EXTRA_OECONF_append_apq8098 = " AUDIO_FEATURE_ENABLED_QAP=true"
EXTRA_OECONF_append_aarch64 = " AUDIO_FEATURE_MS12_64BIT_ENABLED=true"

do_configure[depends] += "virtual/kernel:do_shared_workdir"

do_install_append() {
    install -d ${D}/data/audio
    export PATH="${STAGING_DIR_NATIVE}/usr/bin/python-native:$PATH"
    $(python ${SECIMAGE_BASE}/sectools_builder.py \
     -i ${D}/${libdir}/libdolby_ms12_wrapper.so \
     -t ${D}/ \
     -g dolby \
     --soc_hw_version=0x30020000 \
     --soc_vers=0x3005 \
     --client_id=0x2 --lib_id=0x1 \
     --build_policy_id=MULTIPLE_DEFAULT_SIGN \
     --config ${PROD_CONFIG_DIR}/secimagev2_prod_uie.xml>${D}/secimage_prod.log 2>&1)

    install ${D}/sign_and_encrypt/default/dolby/libdolby_ms12_wrapper.so ${D}/${libdir}/libdolby_ms12_wrapper_prod.so
    rm -rf ${D}/sign_and_encrypt
    rm -rf ${D}/sign

    $(python ${SECIMAGE_BASE}/sectools_builder.py \
     -i ${D}/${libdir}/libdolby_ms12_wrapper.so \
     -t ${D}/ \
     -g dolby \
     --soc_hw_version=0x30020000 \
     --soc_vers=0x3005 \
     --client_id=0x2 --lib_id=0x1 \
     --build_policy_id=MULTIPLE_DEFAULT_SIGN \
     --config ${SECIMAGE_BASE}/config/integration/secimagev2.xml>${D}/secimage_test.log 2>&1)

    install ${D}/sign_and_encrypt/default/dolby/libdolby_ms12_wrapper.so ${D}/${libdir}/
    rm -rf ${D}/sign_and_encrypt
    rm -rf ${D}/sign
    rm -rf ${D}/Sectools_Builder_dolby_log.txt
}

FILES_${PN} += "${userfsdatadir}/*"

FILES_SOLIBSDEV = ""
FILES_${PN} += "${libdir}/*.so"
