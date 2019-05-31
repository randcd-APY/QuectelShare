SUMMARY = "quectel gnss lib"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps/garden-app"

SRC_DIR = "${WORKSPACE}/gps/garden-app"
S = "${WORKDIR}/gps/garden-app"

DEPENDS = "location-api loc-glue location-client-api location-hal-daemon drplugin drplugin-client gnsspps"

FILES_${PN} += "${libdir}/" 

FILES_SOLIBSDEV = ""


do_install() {
		 install -d ${D}${libdir}
		 install -d ${D}${bindir}
		 install -m 0755 lib/* ${D}${libdir}
		 install -m 0755 bin/* ${D}${bindir}
}
