# Packages installed to the root file system of PSM build
require mdm9607-location-image.inc

#Excluding mcm packages
PACKAGE_EXCLUDE += "loc-mcm-test-shim"
PACKAGE_EXCLUDE += "loc-mcm-type-conv"
PACKAGE_EXCLUDE += "mcmlocserver"

#Excluding GPS packages
PACKAGE_EXCLUDE += "loc-launcher"
PACKAGE_EXCLUDE += "dr-ap-service"
PACKAGE_EXCLUDE += "slim-daemon"
PACKAGE_EXCLUDE += "izat-client-api"
PACKAGE_EXCLUDE += "lowi-server"
PACKAGE_EXCLUDE += "lowi-test"
PACKAGE_EXCLUDE += "xtwifi-client"
PACKAGE_EXCLUDE += "xtwifi-inet-agent"
PACKAGE_EXCLUDE += "gtp-ap-le-os-agent"
PACKAGE_EXCLUDE += "nmea-test-app"