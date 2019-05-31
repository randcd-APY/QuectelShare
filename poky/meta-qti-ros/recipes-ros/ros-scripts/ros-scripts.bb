DESCRIPTION = "ROS scripts"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/${LICENSE};md5=550794465ba0ec5312d6919e203a55f9"

PR = "r0"
PV = "1.0"

SRC_URI = "file://ros-env.sh"

PACKAGES = "${PN}"
FILES_${PN} = "/opt/ros/indigo/*"

do_install() {
    dest=/opt/ros/indigo
    install -d ${D}${dest}
    install -m 755 ${WORKDIR}/ros-env.sh ${D}${dest}
}
