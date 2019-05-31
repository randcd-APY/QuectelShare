DESCRIPTION = "Slpi firmware for drones"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti-bsp-prop/files/qcom-licenses/Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

PR = "r1"
PV = "1.0"


FILESPATH =+ "${WORKSPACE}/:"
SRC_URI  = "file://vendor/qcom/proprietary/drones/dronePlugin/flight-slpi/flight_slpi.zip"
SRC_URI  += "file://vendor/qcom/proprietary/drones/dronePlugin/flight-slpi/slpi_firmware_change.sh"

FILES_${PN} += "/lib/firmware/drones_image/*"

INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
INHIBIT_PACKAGE_STRIP = "1"

FIRMWARE_PATH = "${D}/lib/firmware/drones_image/"

PACKAGES = "${PN}"

inherit update-rc.d

INITSCRIPT_NAME = "slpi_firmware_change.sh"
INITSCRIPT_PARAMS = "start 38 S ."

do_install_append() {
   install -d ${FIRMWARE_PATH}
   install -m 0644 ${WORKDIR}/slpi*.* -D ${FIRMWARE_PATH}/
   install -m 0755 ${WORKDIR}/vendor/qcom/proprietary/drones/dronePlugin/flight-slpi/slpi_firmware_change.sh  -D ${D}${sysconfdir}/init.d/slpi_firmware_change.sh
}

INSANE_SKIP_${PN} += "arch"
INSANE_SKIP_${PN} += "textrel"
INSANE_SKIP_${PN} += "libdir"
INSANE_SKIP_${PN} += "ldflags"

