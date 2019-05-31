SECTION = "devel"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://package.xml;beginline=9;endline=9;md5=d566ef916e9dedc494f5f793a6690ba5"

DEPENDS = "catkin geometry-msgs kobuki-msgs kobuki-node message-generation message-runtime python-orocos-kdl sensor-msgs std-msgs"

inherit catkin
require kobuki.inc


