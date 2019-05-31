# SS-MGR Proprietary Packages
IMAGE_INSTALL += "${@base_conditional('USER_BUILD', '1', '', 'atfwd-daemon', d)}"
IMAGE_INSTALL += "${@base_conditional('USER_BUILD', '1', '', 'diag-reboot-app', d)}"
IMAGE_INSTALL += "qmi-shutdown-modem"
IMAGE_INSTALL += "atfwd-quectel"
