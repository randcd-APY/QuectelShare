SECTION = "devel"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://package.xml;beginline=9;endline=9;md5=e838a4f2cec626b8f1f24b8c74f6bd04"

DEPENDS = "angles capabilities catkin diagnostic-aggregator diagnostic-msgs diagnostic-updater ecl-exceptions ecl-sigslots ecl-streams ecl-threads geometry-msgs kobuki-rapps nav-msgs nodelet pluginlib roscpp rospy sensor-msgs std-msgs tf kobuki-driver diagnostic-updater kobuki-keyop kobuki-safety-controller"

inherit catkin
require kobuki.inc


