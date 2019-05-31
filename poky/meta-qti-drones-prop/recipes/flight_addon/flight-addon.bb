DESCRIPTION = "Flight Add-on Utilities"
LICENSE          = "QUALCOMM-TECHNOLOGY-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti-bsp-prop/files/qcom-licenses/Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

PR = "r1"
PV = "1.0"

FILESPATH =+ "${WORKSPACE}:"

SRC_URI  = "file://vendor/qcom/proprietary/drones/dronePlugin/flight-addon/flight_controller.zip"
SRC_URI  += "file://vendor/qcom/proprietary/drones/dronePlugin/flight-addon/libgnss_test_skel.so"
SRC_DIR   = "${WORKSPACE}/vendor/qcom/proprietary/drones/dronePlugin/flight-addon"

INSANE_SKIP_${PN} += "installed-vs-shipped"

FILES_${PN} += "/usr/bin/*"
FILES_${PN} += "/usr/include/sensor-imu/*"
FILES_${PN} += "/usr/lib/*"
FILES_${PN} += "/usr/lib/rfsa/adsp/*"
FILES_${PN} += "/usr/tests/*"

INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_DEFAULT_DEPS = "1"
INHIBIT_SYSROOT_STRIP = "1"

PACKAGES = "${PN}"

do_install_append() {
   dest=/usr/bin/
   install -d ${D}${dest}
   install  ${WORKDIR}/krait/apps/* ${D}${dest}

   dest=/usr/include/sensor-imu/
   install -d ${D}${dest}
   install ${WORKDIR}/krait/inc/* ${D}${dest}

   dest=${COREBASE}/../FEAT-API-LNX-IMU/apps_proc
   install -d ${dest}
   cp -rf ${WORKDIR}/krait/inc ${dest}

   dest=/usr/lib/rfsa/adsp/
   install -d ${D}${dest}
   install ${WORKDIR}/hexagon/libs/* ${D}${dest}
   install ${WORKDIR}/vendor/qcom/proprietary/drones/dronePlugin/flight-addon/libgnss_test_skel.so ${D}${dest}

   dest=/usr/lib/
   install -d ${D}${dest}
   install ${WORKDIR}/krait/libs/* ${D}${dest}

   dest=/usr/tests/
   install -d ${D}${dest}
   install ${WORKDIR}/krait/tests/* ${D}${dest}
}

INSANE_SKIP_${PN} += "arch"
INSANE_SKIP_${PN} += "installed-vs-shipped"
INSANE_SKIP_${PN} += "textrel"
INSANE_SKIP_${PN} += "libdir"
INSANE_SKIP_${PN} += "ldflags"
INSANE_SKIP_${PN} += "already-stripped"
