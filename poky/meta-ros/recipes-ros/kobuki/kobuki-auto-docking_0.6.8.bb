SECTION = "devel"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://package.xml;beginline=9;endline=9;md5=53e9d23d39ef914eb04c159511d11955"

DEPENDS = "actionlib actionlib-msgs catkin ecl-geometry ecl-linear-algebra ecl-threads geometry-msgs kobuki-msgs nav-msgs nodelet pluginlib roscpp rospy std-msgs yocs-cmd-vel-mux kdl-conversions kobuki-msgs kobuki-dock-drive message-filters"

inherit catkin
require kobuki.inc


