# Packages installed to the root file system of PSM build
require mdm9607-qti-image.inc

PACKAGE_EXCLUDE += "ftm"

#Excluding mcm packages 
PACKAGE_EXCLUDE += "mcm-core"

PACKAGE_EXCLUDE += "pdc-daemon"
PACKAGE_EXCLUDE += "rmtstorage"

#Excluding Wlan packages
PACKAGE_EXCLUDE += "wap-fota-daemon"

#Excluding Flouride BT Packages
PACKAGE_EXCLUDE += "hci-qcomm-init"
PACKAGE_EXCLUDE += "qc-bt-daemon"

#Excluding Synergy BT packages
PACKAGE_EXCLUDE += "synergy"

