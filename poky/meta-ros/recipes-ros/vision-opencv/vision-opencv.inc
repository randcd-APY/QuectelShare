SRC_URI = "https://github.com/ros-perception/${ROS_SPN}/archive/1.12.8.tar.gz;downloadfilename=vision_opencv-1.12.8.tar.gz"
SRC_URI[md5sum] = "0a663fc3ac4cb88869fcdf6a8f813cb9"
SRC_URI[sha256sum] = "5cf7e60824cd73be3dfa3387299d79a1c45a7e4a621077788ed5b4e2ed875f44"

S = "${WORKDIR}/${ROS_SP}/${ROS_BPN}"

inherit catkin

ROS_SPN = "vision_opencv"
