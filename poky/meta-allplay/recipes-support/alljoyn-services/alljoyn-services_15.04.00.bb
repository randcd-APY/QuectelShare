LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://services/base/README.md;md5=0aa4c355b53fc9ab161679cd5ef5b6a1"
DEPENDS = "alljoyn"
RDEPENDS_${PN} = "alljoyn"

SRC_URI = " \
	https://www.codeaurora.org/mirrored_source/quic/qsdk/alljoyn-services-${PV}-src.tar.gz \
	file://010-make-services-common-a-dependency-of-other-libs.patch \
	"
SRC_URI[md5sum] = "fab340606d98f83d7d718a30aede033b"
SRC_URI[sha256sum] = "a6deb8d387b11bac58808a0073bb799bb805a9372d989ec78c6b0e33e4a794a8"

S = "${WORKDIR}/alljoyn-services-${PV}-src"

ALLJOYN_BINDINGS = "cpp,"
ALLJOYN_BUILD_SERVICES_SAMPLES = "off"
# Temporarily use debug config for debug logging while we investigate SKIFTAAM-648
ALLJOYN_BUILD_VARIANT = "debug"
ALLJOYN_SERVICES = "config,notification,onboarding,"
CFLAGS += "-fPIC"
CPPFLAGS += "-fPIC"
PKG_INSTALL_DIR = "${S}/core/alljoyn/build/openwrt/openwrt/${ALLJOYN_BUILD_VARIANT}/dist"

inherit scons

EXTRA_OESCONS = "-C ${S}/core/alljoyn \
                OS=openwrt \
                CPU=openwrt \
                BINDINGS=${ALLJOYN_BINDINGS} \
                BUILD_SERVICES_SAMPLES=${ALLJOYN_BUILD_SERVICES_SAMPLES} \
                VARIANT=${ALLJOYN_BUILD_VARIANT} \
                SERVICES=${ALLJOYN_SERVICES} \
                BR=off \
                ICE=off \
                BT=off \
                WS=off \
                TARGET_CC="${CC}" \
                TARGET_CXX="${CXX}" \
                TARGET_CFLAGS="${CFLAGS}" \
                TARGET_CPPFLAGS="${CPPFLAGS}" \
                TARGET_PATH="${PATH}" \
                TARGET_LINKFLAGS="${LDFLAGS} -lrt" \
                TARGET_LINK="${CC}" \
                TARGET_AR="${AR}" \
                TARGET_RANLIB="${RANLIB}" \
                STAGING_DIR="${STAGING_DIR}" \
                "

do_install() {
    install -d ${D}${libdir}
    install -m 0755 ${PKG_INSTALL_DIR}/config/lib/liballjoyn_config.so ${D}${libdir}
    install -d ${D}${includedir}/alljoyn/config
    install -m 0644 ${PKG_INSTALL_DIR}/config/inc/alljoyn/config/*.h ${D}${includedir}/alljoyn/config

    install -m 0755 ${PKG_INSTALL_DIR}/notification/lib/liballjoyn_notification.so ${D}${libdir}
    install -d ${D}${includedir}/alljoyn/notification
    install -m 0644 ${PKG_INSTALL_DIR}/notification/inc/alljoyn/notification/*.h ${D}${includedir}/alljoyn/notification

    install -m 0755 ${PKG_INSTALL_DIR}/onboarding/lib/liballjoyn_onboarding.so ${D}${libdir}
    install -d ${D}${includedir}/alljoyn/onboarding
    install -m 0644 ${PKG_INSTALL_DIR}/onboarding/inc/alljoyn/onboarding/*.h ${D}${includedir}/alljoyn/onboarding

    install -m 0755 ${PKG_INSTALL_DIR}/services_common/lib/liballjoyn_services_common.so ${D}${libdir}
    install -d ${D}${includedir}/alljoyn/services_common
    install -m 0644 ${PKG_INSTALL_DIR}/services_common/inc/alljoyn/services_common/*.h ${D}${includedir}/alljoyn/services_common
}

FILES_SOLIBSDEV = ""
FILES_${PN} += "${libdir}/*"
