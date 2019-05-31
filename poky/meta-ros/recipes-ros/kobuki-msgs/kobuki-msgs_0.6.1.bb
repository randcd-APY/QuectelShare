SECTION = "devel"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://package.xml;beginline=9;endline=9;md5=78da5bfe8cbdd710c9477bc640b57f69"

PROVIDES = "kobuki-msgs" 

DEPENDS = "actionlib-msgs catkin message-generation message-runtime std-msgs"
# https://github.com/yujinrobot/kobuki_msgs/archive/0.6.1.tar.gz
SRC_URI = "https://github.com/yujinrobot/kobuki_msgs/archive/0.6.1.tar.gz;downloadfilename=kobuki_msgs.tar.gz"

SRC_URI[md5sum] = "9a2ebd9de77d546636027b9ddeb5dd84"
SRC_URI[sha256sum] = "e8814159bec34b707936c69d06ca68eef02b5859f449b3c917ec236bf44cfaf9"

S = "${WORKDIR}/${ROS_SP}"

inherit catkin
