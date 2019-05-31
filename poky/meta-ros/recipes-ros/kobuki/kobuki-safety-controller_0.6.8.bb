SECTION = "devel"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://package.xml;beginline=9;endline=9;md5=1ffa6afae980d20b989794057fdf02ce"

DEPENDS = "catkin ecl-threads geometry-msgs kobuki-msgs nodelet pluginlib roscpp std-msgs yocs-controllers "

inherit catkin
require kobuki.inc


