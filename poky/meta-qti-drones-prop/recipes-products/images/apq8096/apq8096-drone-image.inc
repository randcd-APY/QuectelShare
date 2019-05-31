#Include the drone settings

IMAGE_INSTALL += "adsp-sensor-test"
PREBUILT_DIR_PRESENT = "${@os.path.exists('${WORKSPACE}/vendor/qcom/proprietary/drones/drones-prebuilts')}"
IMAGE_INSTALL += "${@base_conditional('PREBUILT_DIR_PRESENT', 'True', 'dronesprebuilts', '', d)}"
#IMAGE_INSTALL += "flight-slpi"
IMAGE_INSTALL += "lib32-flight-addon"
