inherit autotools qcommon qlicense qprebuilt
require ${COREBASE}/meta-qti-location-prop/recipes/include/common-location-prop-gps-defines.inc

DESCRIPTION = "GPS Geofence"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://gps-noship/geofence/"
SRC_DIR = "${WORKSPACE}/gps-noship/geofence/"
S = "${WORKDIR}/gps-noship/geofence"

DEPENDS = "lbs-core"

CXXINC  = "-I${STAGING_INCDIR}/c++"
CXXINC += "-I${STAGING_INCDIR}/c++/${TARGET_SYS}"
CXXFLAGS ="${CXXINC}"

EXTRA_OECONF = "--with-glib"

PR = "r1"

LDFLAGS += "-Wl,--build-id=sha1"

PACKAGES = "${PN}"
#Disable the split of debug information into -dbg files
INHIBIT_PACKAGE_DEBUG_SPLIT = "1"

FILES_${PN} = "${libdir}/*"
FILES_${PN} += "/usr/include/*"
# The geofence package contains symlinks that trip up insane
INSANE_SKIP_${PN} = "dev-so"
