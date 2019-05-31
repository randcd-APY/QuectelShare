DESCRIPTION = "This package contains step-by-step tutorials written in C++ for learning ROS."
SECTION = "devel"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/\
${LICENSE};md5=3775480a712fc46a69647678acb234cb"

DEPENDS += "message-generation roscpp rosconsole roscpp-serialization rostime std-msgs libRoboticsCamera"
RDEPENDS_${PN} += "adreno lib-robotics-camera libssl libcrypto zlib"

FILESPATH =+ "${WORKSPACE}/vendor/qcom/opensource/ros:"
SRC_URI = "file://ros-tests"
PR = "r0"
S = "${WORKDIR}/ros-tests"

inherit catkin
#ROS_SPN = "ros_tests"
EXTRA_OECMAKE = " -DKERNEL_HEADERS=${STAGING_KERNEL_BUILDDIR}/usr/include"

do_deploy_to_target() {
   # Install the resulting package on-target
   adb shell mkdir -p /home/root/pkgDeployDir
   adb push ${DEPLOY_DIR_IPK}/${PACKAGE_ARCH}/${PN}_*ipk /home/root/pkgDeployDir
   adb shell opkg remove ${PN}
   adb shell opkg install /home/root/pkgDeployDir/${PN}*
}

do_install_append() {
    cd ${D}/opt/ros/indigo/share/ros_tests/launch/
    ln -s client_server.launch ion.launch
}
addtask deploy_to_target after do_package_write_ipk
