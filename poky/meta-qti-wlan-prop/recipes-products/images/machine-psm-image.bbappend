# wlan proprietary Packages
include ${BASEMACHINE}/${BASEMACHINE}-wlan-image.inc

PACKAGE_EXCLUDE_append_mdm9607 += "wlan-services qcacld-utils"
