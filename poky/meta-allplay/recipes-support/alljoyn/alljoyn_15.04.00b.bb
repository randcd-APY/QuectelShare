LICENSE = "ISC"
LIC_FILES_CHKSUM = "file://alljoyn_core/docs/legal/legal_notice.html;md5=fca32c256e32a2c51dbe0e481ecc6519"

PR = "r1"

SRC_URI = "https://www.codeaurora.org/mirrored_source/quic/qsdk/alljoyn-${PV}-src.tar.gz"
SRC_URI += "file://alljoyn.conf \
            file://alljoyn.service \
            file://001-ASACORE-2178-Add-extra-checks-for-connection-validat.patch \
            file://002-Update_AJ_logs_header.patch \
            file://003-Increase_alarm_number.patch \
            file://004-Implement_GetAliases.patch \
            file://005-ASACORE-1999-Do-not-limit-number-of-messages-that-can-.patch \
            file://006-ASACORE-2221-Connection-lookup-should-precede-check-.patch \
            file://100-Check-debug-log-format.patch \
            file://110-Support-GetHostInfo-from-session-host.patch \
            file://120-static-ip-discovery.patch \
            file://131-Delete_old_interfaces.patch \
            file://132-Prioritize_last_address_for_JoinSession.patch \
            file://133-Shorter_socket_connect_timeout.patch \
            file://200-disable-deprecated-warnings.patch \
            file://201-change-default-homedir.patch \
            "
SRC_URI[md5sum] = "5abf1ac640258eeb04617e39018536a7"
SRC_URI[sha256sum] = "749e820ac5379538025218c6507cc813ee6b24da55e0a6340474d694811fecf4"

S = "${WORKDIR}/${BPN}-${PV}-src"

DEPENDS = "openssl libcap"

#Specify Run time Depedencies
RDEPENDS_${PN} = "openssl libcap"

ALLJOYN_BINDINGS = "cpp,c,"
ALLJOYN_SERVICES = "about,"
ALLJOYN_BUILD_SERVICES_SAMPLES = "off"
# Temporarily use debug config for debug logging while we investigate SKIFTAAM-648
ALLJOYN_BUILD_VARIANT = "debug"
CFLAGS += "-fPIC"
CPPFLAGS += "-fPIC"

inherit scons pkgconfig systemd

PKG_INSTALL_DIR = "${S}/build/openwrt/openwrt/${ALLJOYN_BUILD_VARIANT}/dist"

# Specify any options you want to pass to scons using EXTRA_OESCONS:
EXTRA_OESCONS = "-C ${S} \
                OS=openwrt \
                CPU=openwrt \
                BINDINGS=${ALLJOYN_BINDINGS} \
                SERVICES=${ALLJOYN_SERVICES} \
                BUILD_SERVICES_SAMPLES=${ALLJOYN_BUILD_SERVICES_SAMPLES} \
                VARIANT=${ALLJOYN_BUILD_VARIANT} \
                BR=off \
                ICE=off \
                POLICYDB=off \
                WS=off \
                TARGET_CC="${CC}" \
                TARGET_CXX="${CXX}" \
                TARGET_CFLAGS="${CFLAGS}" \
                TARGET_CPPFLAGS="${CPPFLAGS}" \
                TARGET_PATH="${PATH}" \
                TARGET_LINKFLAGS="${LDFLAGS}" \
                TARGET_LINK="${CC}" \
                TARGET_AR="${AR}" \
                TARGET_RANLIB="${RANLIB}" \
                STAGING_DIR="${STAGING_DIR}" \
                "
do_install() {
    install -d ${D}${bindir}
    install -d ${D}${libdir}
    install -m 0755 ${PKG_INSTALL_DIR}/cpp/bin/alljoyn-daemon ${D}${bindir}
    install -m 0755 ${PKG_INSTALL_DIR}/cpp/lib/liballjoyn.so ${D}${libdir}
    install -m 0755 ${PKG_INSTALL_DIR}/c/lib/liballjoyn_c.so ${D}${libdir}

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/alljoyn.service -D ${D}${systemd_system_unitdir}

    install -d ${D}/etc/alljoyn
    install -D -m 0600 ${WORKDIR}/alljoyn.conf ${D}/etc/alljoyn/alljoyn.conf

    #Adding AllJoyn Headers to package include directory
    install -d ${D}${includedir}/alljoyn
    install -m 0644 ${PKG_INSTALL_DIR}/cpp/inc/alljoyn/*.h ${D}${includedir}/alljoyn
    install -d ${D}${includedir}/qcc
    install -m 0644 ${PKG_INSTALL_DIR}/cpp/inc/qcc/*.h ${D}${includedir}/qcc
    install -d ${D}${includedir}/qcc/posix
    install -m 0644 ${PKG_INSTALL_DIR}/cpp/inc/qcc/posix/*.h ${D}${includedir}/qcc/posix

    #Adding alljoyn about
    install -d ${D}${includedir}/alljoyn/about
    install -m 0644 ${PKG_INSTALL_DIR}/cpp/inc/alljoyn/about/*.h ${D}${includedir}/alljoyn/about
    install -m 0755 ${PKG_INSTALL_DIR}/cpp/lib/liballjoyn_about.so ${D}${libdir}
}

FILES_SOLIBSDEV = ""
SOLIBS = ".so"
FILES_${PN} += "${systemd_system_unitdir}"

SYSTEMD_SERVICE_${PN} = "alljoyn.service"
