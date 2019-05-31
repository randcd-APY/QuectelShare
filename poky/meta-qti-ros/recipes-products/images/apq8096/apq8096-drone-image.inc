#Include the drone settings

IMAGE_INSTALL += "${@base_contains('DISTRO_FEATURES', 'ROS', 'packagegroup-ros-qti', '', d)}"
IMAGE_INSTALL += "${@base_contains('DISTRO_FEATURES', 'ROS', 'lib32-ros-tests', '', d)}"
