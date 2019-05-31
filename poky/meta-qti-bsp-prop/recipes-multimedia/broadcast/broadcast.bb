inherit qcommon autotools qlicense

DESCRIPTION = "DVB (Digital Video Broadcasting) streaming functionality"

PR = "r0"

DEPENDS = "virtual/kernel audiohal"

SRC_URI = "file://vendor/qcom/proprietary/media-hub/broadcast/"
S = "${WORKDIR}/vendor/qcom/proprietary/media-hub/broadcast/"

CFLAGS += "-I${STAGING_KERNEL_BUILDDIR}/usr/include"
do_configure[depends] += "virtual/kernel:do_shared_workdir"
