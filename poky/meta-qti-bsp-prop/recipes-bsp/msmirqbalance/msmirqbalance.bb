inherit autotools qcommon update-rc.d qlicense qprebuilt

DESCRIPTION = "msm_irqbalance"

PR = "r0"

FILESPATH =+ "${WORKSPACE}/vendor/qcom/proprietary/:"
SRC_URI = "file://msmirqbalance/"

SRC_DIR = "${WORKSPACE}/vendor/qcom/proprietary/msmirqbalance"
S = "${WORKDIR}/msmirqbalance"

DEPENDS = "system-core"


INITSCRIPT_NAME = "msmirqbalance"
INITSCRIPT_PARAMS = "start 40 2 3 4 5 . stop 60 0 1 6 ."

CFLAGS += "-I${WORKSPACE}/system/core/include"

do_install() {
       install -m 0755 ${S}/msm-irqbalance -D ${D}${bindir}/msm-irqbalance
       install -m 0755 ${S}/start_msm-irqbalance_le -D ${D}${sysconfdir}/init.d/msmirqbalance
       install -m 0755 ${S}/msm_irqbalance-8053.conf -D ${D}${sysconfdir}/msm_irqbalance-8053.conf
}
