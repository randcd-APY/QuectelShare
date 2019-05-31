# Data open source Packages
require msm8909-data-image.inc
IMAGE_INSTALL += "${@oe.utils.conditional('PRODUCT','robot-rome', 'data', '', d)}"
IMAGE_INSTALL += "${@oe.utils.conditional('PRODUCT','robot-rome', 'iputils', '', d)}"
