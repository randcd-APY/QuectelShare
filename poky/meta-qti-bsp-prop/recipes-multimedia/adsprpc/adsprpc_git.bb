inherit qcommon qlicense qprebuilt update-rc.d systemd pkgconfig useradd

SUMMARY = "adsprpc daemon"

DEPENDS += "system-core"

FILESPATH =+ "${WORKSPACE}/:"
SRC_URI   = "file://adsprpc"

# sysvinit specific
SRC_URI += "file://start_adsprpcd"
SRC_URI += "file://start_sdsprpcd"
SRC_URI += "file://start_mdsprpcd"
SRC_URI += "file://non-root_start_adsprpcd"

# system services
SRC_URI += "file://adsprpcd.service"
SRC_URI += "file://adsprpcd_rootpd.service"
SRC_URI += "file://adsprpcd_audiopd.service"
SRC_URI += "file://adsprpcd_sensorspd.service"
SRC_URI += "file://sdsprpcd.service"
SRC_URI += "file://mdsprpcd.service"
SRC_URI += "file://cdsprpcd.service"

# Enable cdsp at boot
SRC_URI += "file://cdsp.service"
SRC_URI += "file://cdsp.sh"

#Add Udev Rules
SRC_URI += "file://dsp.rules"

SRC_DIR = "${WORKSPACE}/adsprpc"
S = "${WORKDIR}/adsprpc"
PR = "r1"

# build and copy all libraries and binaries as part of rootfs.
EXTRA_OECONF += "--enable-sdsprpc"
EXTRA_OECONF += "--enable-mdsprpc"
EXTRA_OECONF += "--enable-cdsprpc"

INITSCRIPT_PACKAGES           = "${PN}"
INITSCRIPT_PACKAGES_apq8096   = "${PN} ${PN}-sdsp"
INITSCRIPT_NAME_${PN}         = "adsprpcd"
INITSCRIPT_NAME_${PN}_msm8909 = "mdsprpcd"
INITSCRIPT_PARAMS_${PN}       = "start 70 2 3 4 5 S . stop 30 0 1 6 ."
INITSCRIPT_NAME_${PN}-sdsp    = "sdsprpcd"
INITSCRIPT_PARAMS_${PN}-sdsp  = "start 70 2 3 4 5 . stop 69 0 1 6 ."

# Enable the below services by default immediately after boot.
SYSTEMD_SERVICE_${PN}          = "adsprpcd.service"

SYSTEMD_SERVICE_${PN}_apq8096 += "adsprpcd.service"
SYSTEMD_SERVICE_${PN}_apq8096 += "sdsprpcd.service"
SYSTEMD_SERVICE_${PN}_msm8909 += "mdsprpcd.service"

SYSTEMD_SERVICE_${PN}_qcs605 += "cdsprpcd.service"
SYSTEMD_SERVICE_${PN}_qcs605 += "cdsp.service"
SYSTEMD_SERVICE_${PN}_qcs605 += "adsprpcd_rootpd.service"
SYSTEMD_SERVICE_${PN}_qcs605 += "adsprpcd_audiopd.service"
SYSTEMD_SERVICE_${PN}_qcs605 += "adsprpcd_sensorspd.service"

SYSTEMD_SERVICE_${PN}_qcs405 += "cdsprpcd.service"
SYSTEMD_SERVICE_${PN}_qcs405 += "cdsp.service"
SYSTEMD_SERVICE_${PN}_qcs405 += "adsprpcd.service"
SYSTEMD_SERVICE_${PN}_qcs405 += "adsprpcd_audiopd.service"
SYSTEMD_SERVICE_${PN}_qcs405 += "adsprpcd_sensorspd.service"

SYSTEMD_SERVICE_${PN}_qcs403-som2 += "cdsprpcd.service"
SYSTEMD_SERVICE_${PN}_qcs403-som2 += "cdsp.service"
SYSTEMD_SERVICE_${PN}_qcs403-som2 += "adsprpcd.service"
SYSTEMD_SERVICE_${PN}_qcs403-som2 += "adsprpcd_audiopd.service"

SYSTEMD_SERVICE_${PN}_sdmsteppe += "cdsprpcd.service"
SYSTEMD_SERVICE_${PN}_sdmsteppe += "cdsp.service"
SYSTEMD_SERVICE_${PN}_sdmsteppe += "adsprpcd.service"
SYSTEMD_SERVICE_${PN}_sdmsteppe += "adsprpcd_audiopd.service"
SYSTEMD_SERVICE_${PN}_sdmsteppe += "adsprpcd_sensorspd.service"

QPERM_SERVICE = "${WORKDIR}/adsprpcd.service ${WORKDIR}/adsprpcd_rootpd.service ${WORKDIR}/adsprpcd_audiopd.service ${WORKDIR}/adsprpcd_sensorspd.service ${WORKDIR}/mdsprpcd.service ${WORKDIR}/cdsprpcd.service ${WORKDIR}/sdsprpcd.service"

do_install_append () {
if [ "${@bb.utils.contains('DISTRO_FEATURES','systemd', 'true', 'false', d)}" == "false" ]; then
	install -d ${D}${sysconfdir}/init.d
	install -m 0755 ${WORKDIR}/start_${INITSCRIPT_NAME_${PN}} -D ${D}${sysconfdir}/init.d/${INITSCRIPT_NAME_${PN}}
else
	# Install systemd unit files
	install -d ${D}${systemd_unitdir}/system
	install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants
	install -m 0644 ${WORKDIR}/adsprpcd.service ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/adsprpcd_rootpd.service ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/adsprpcd_audiopd.service ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/adsprpcd_sensorspd.service ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/mdsprpcd.service ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/cdsprpcd.service ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/sdsprpcd.service ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/cdsp.service ${D}${systemd_unitdir}/system
	install -m 0744 ${WORKDIR}/cdsp.sh ${D}${bindir}
        if [ "${QPERMISSIONS_ENABLE}" == "1" ]; then
            install -m 0755 ${WORKDIR}/dsp.rules -D ${D}${sysconfdir}/udev/rules.d/dsp.rules
        fi
fi
}
do_install_append_apq8053() {
if [ "${@bb.utils.contains('DISTRO_FEATURES','systemd', 'true', 'false', d)}" == "false" ]; then
	install -m 0755 ${WORKDIR}/non-root_start_${INITSCRIPT_NAME_${PN}} -D ${D}${sysconfdir}/init.d/${INITSCRIPT_NAME_${PN}}
else
	# Install systemd unit files
	install -d ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/${INITSCRIPT_NAME_${PN}}.service ${D}${systemd_unitdir}/system
fi
}

do_install_append_apq8096 () {
if [ "${@bb.utils.contains('DISTRO_FEATURES','systemd', 'true', 'false', d)}" == "false" ]; then
	install -m 0755 ${WORKDIR}/start_sdsprpcd -D ${D}${sysconfdir}/init.d/sdsprpcd
	install -m 0755 ${WORKDIR}/start_adsprpcd -D ${D}${sysconfdir}/init.d/adsprpcd
else
	#Install systemd unit files
	install -m 0644 ${WORKDIR}/sdsprpcd.service ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/adsprpcd.service ${D}${systemd_unitdir}/system
fi
}

PACKAGES_append_apq8096 = " ${PN}-sdsp"
FILES_${PN}-sdsp-dbg  = "${libdir}/.debug/libsdsp* ${bindir}/.debug/sdsprpcd"
FILES_${PN}-sdsp = "${sysconfdir}/init.d/sdsprpcd ${bindir}/sdsprpcd ${libdir}/libsdsp*.so ${libdir}/libsdsp*.so.*"

FILES_${PN}-dbg = "${libdir}/.debug/* ${bindir}/.debug/*"
FILES_${PN}-dev = "${libdir}/*.la ${includedir}"

FILES_${PN}  = "${libdir}/libadsp*.so ${libdir}/libadsp*.so.* ${bindir}/adsprpcd"
FILES_${PN} += "${libdir}/libmdsp*.so ${libdir}/libmdsp*.so.* ${bindir}/mdsprpcd"
FILES_${PN} += "${libdir}/libcdsp*.so ${libdir}/libcdsp*.so.* ${bindir}/cdsprpcd"
FILES_${PN} += "${libdir}/libsdsp*.so ${libdir}/libsdsp*.so.* ${bindir}/sdsprpcd"
FILES_${PN} += "${libdir}/pkgconfig/  ${systemd_unitdir}/system/*"
FILES_${PN} += "${sysconfdir}/*  ${libdir}/pkgconfig/  ${bindir}"
