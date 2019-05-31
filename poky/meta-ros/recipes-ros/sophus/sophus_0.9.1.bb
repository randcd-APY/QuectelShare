SECTION = "devel"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://package.xml;beginline=9;endline=9;md5=d25939bd242aba3a145910b448c43b6d"

PROVIDES = "sophus" 

DEPENDS = "catkin ecl-eigen"
SRC_URI = "https://github.com/stonier/sophus/archive/${PV}.tar.gz;downloadfilename=${ROS_SP}.tar.gz"

SRC_URI[md5sum] = "de77d9f4b769df91bd57c5224f7f1b88"
SRC_URI[sha256sum] = "962165b5233c5d4b4d1f6c36ea77e6f3d004b9fff907f617f9952f84534177cc"

S = "${WORKDIR}/${ROS_SP}"

inherit catkin
