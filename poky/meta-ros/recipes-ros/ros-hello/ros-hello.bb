SECTION = "devel"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://package.xml;beginline=12;endline=10;md5=d41d8cd98f00b204e9800998ecf8427e"

DEPENDS = "actionlib-msgs geometry-msgs message-generation roscpp"

SRC_URI = "https://github.com/ander-ansuategi/ros-hello-world/archive/master.tar.gz;downloadfilename=ros-hello-world-master.tar.gz"

#SRC_URI[md5sum] = "2aa8afbfbc61473a541633afa761fac9"
#SRC_URI[sha256sum] = "a1fe0af70d9b694354058824fdc53055acd77d60519879d10fbd932f88c7dd04"
SRC_URI[md5sum] = "a7d61d47906da091e07368b320201432"
SRC_URI[sha256sum] = "87a86aff10dd99cff75ac11ed93a4753f6378170fd29897983b18de753711e11"

S = "${WORKDIR}/ros-hello-world-master/hello_world"

inherit catkin
