# PACKAGES to use -fPIC in security flags instead of -fPIE
# GPS Prop packages
SECURITY_CFLAGS_pn-asn1c-cper = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-asn1c-crt = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-asn1c-rtx = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-data-items  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-dr-ap-service  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-drplugin = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-drplugin-client = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-drplugin-server = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-event-observer = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-garden-app = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-garden-test-interfaces = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-gdtap-adapter = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-gnsspps = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-gtp-ap-le-os-agent = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-izat-client-api = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-izat-core = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-lbs-core = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-location-flp = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-location-geofence = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-location-service  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-loc-base-util = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-loc-externaldr = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-loc-externaldrcore = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-loc-glue  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-loc-launcher = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-loc-mq-client = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-lowi-client  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-lowi-common  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-lowi-server  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-lowi-test  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-nmea-test-app  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-slim-client  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-slim-common  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-slim-daemon  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-slim-utils  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-ulp2  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-xtwifi-client  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-xtwifi-inet-agent  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-loc-mcm-test-shim  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-loc-mcm-type-conv  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-mcmlocserver  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-loc-eng-hub  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-loc-glue  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-loc-hal-test-shim  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-location-service  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-data-items  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-izat-remote-api-test  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-engine-plugin  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-location-client-api  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-engine-simulator  = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-loc-epdr = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-telephony-library = "${SECURITY_PIC_CFLAGS}"

# BT prop packages
SECURITY_CFLAGS_pn-hci-qcomm-init = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-qc-bt-daemon = "${SECURITY_PIC_CFLAGS}"

SECURITY_CFLAGS_pn-data = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-display-noship = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-scve-noship = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-lib32-scve-noship = "${SECURITY_PIC_CFLAGS}"

# Camera packages
SECURITY_CFLAGS_pn-camx = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-chicdk = "${SECURITY_PIC_CFLAGS}"
SECURITY_CFLAGS_pn-chicdkgenerator = "${SECURITY_PIC_CFLAGS}"

#
### PACKAGES which can't compile with security format strings check
#
SECURITY_CFLAGS_remove_pn-data      = "${SECURITY_STRINGFORMAT}"
SECURITY_CFLAGS_remove_pn-mcm-core  = "${SECURITY_STRINGFORMAT}"
SECURITY_CFLAGS_remove_pn-camx      = "${SECURITY_STRINGFORMAT}"
SECURITY_CFLAGS_remove_pn-chicdk    = "${SECURITY_STRINGFORMAT}"
SECURITY_CFLAGS_remove_pn-chicdkgenerator = "${SECURITY_STRINGFORMAT}"

#
### PACKAGES which can't compile with stack-protector
#
SECURITY_CFLAGS_remove_pn-adreno = "-fstack-protector-strong"

#
### PACKAGES which can't compile with _FORTIFY_SOURCE
#
SECURITY_CFLAGS_remove_pn-qcacld-utils= "-D_FORTIFY_SOURCE=2"
