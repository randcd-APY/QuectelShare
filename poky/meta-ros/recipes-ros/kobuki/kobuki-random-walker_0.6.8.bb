SECTION = "devel"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://package.xml;beginline=9;endline=9;md5=d566ef916e9dedc494f5f793a6690ba5"

DEPENDS = "catkin ecl-threads geometry-msgs kobuki-msgs nodelet pluginlib roscpp std-msgs yocs-cmd-vel-mux yocs-controllers"

inherit catkin
require kobuki.inc


