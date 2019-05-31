DESCRIPTION = "ros-eagle package group"
LICENSE = "MIT"

inherit packagegroup

PACKAGES = "${PN}"

RDEPENDS_${PN} = "\
    packagegroup-ros-comm \
    actionlib \
    bond \
    bondpy \
    bondcpp \
    smclib \
    class-loader \
    actionlib-msgs \
    diagnostic-msgs \
    nav-msgs \
    geometry-msgs \
    sensor-msgs \
    shape-msgs \
    stereo-msgs \
    trajectory-msgs \
    visualization-msgs \
    dynamic-reconfigure \
    tf2 \
    tf2-msgs \
    tf2-py \
    tf2-ros \
    tf \
    image-transport \
    nodelet-topic-tools \
    nodelet \
    pluginlib \
    cmake-modules \
    rosconsole-bridge \
    ros-scripts \
    camera-info-manager \
    python-rosdep \
    python-json \
    python-argparse \
    git \
    python-empy \
    cv-bridge \
    diagnostic-aggregator \
    opencv \
    kobuki-capabilities \
    kobuki-safety-controller \
    kobuki-controller-tutorial \
    kobuki-rapps \
    kobuki-keyop \
    kobuki-node \
    kobuki-random-walker \
    kobuki-auto-docking \
    kobuki-bumper2pc \
    kobuki \
    kobuki-core \
    kobuki-dock-drive \
    kobuki-driver \
    kobuki-msgs \
    qhull \
    libcryptopp \
"  
