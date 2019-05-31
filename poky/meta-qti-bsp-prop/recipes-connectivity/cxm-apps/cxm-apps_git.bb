inherit autotools qlicense

DESCRIPTION = "Coex Apps"
PR = "r3"

DEPENDS = "qmi-framework ath6kl-proprietary compat-wireless-ath6kl libnl"

#If 9615-cdp then use only qmi-framework as dependency
DEPENDS_9615-cdp = "qmi-framework"

S= "${WORKDIR}/cxm-apps"

SRC_URI = "file://${WORKSPACE}/cxm-apps"

EXTRA_OECONF += "--enable-target=${BASEMACHINE}"
