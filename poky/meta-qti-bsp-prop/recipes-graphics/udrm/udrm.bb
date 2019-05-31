inherit autotools

DESCRIPTION = "DRM Helper APIs"
HOMEPAGE = "http://support.cdmatech.com"
LICENSE = "Qualcomm-Technologies-Inc.-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
Qualcomm-Technologies-Inc.-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"

PR = "r3"

SRC_URI = "file://${WORKSPACE}/udrm"
S = "${WORKDIR}/udrm"

DEPENDS += "libdrm"
DEPENDS += "virtual/kernel"

EXTRA_OECONF_append += " --with-kernel-headers=${STAGING_KERNEL_DIR}/usr/include"

PACKAGE_ARCH = "${MACHINE_ARCH}"

LEAD_SONAME="libudrm.so"
FILES_${PN} += "/usr/lib/udrm/libudrm.so"

CFLAGS += " -Wno-error "

do_install() {
   install -d ${D}/usr/include
   install -d ${D}/usr/include/udrm
   install -m 0644 ${S}/src/udrm.h ${D}/usr/include/udrm/

   install -d ${D}/usr/lib
   install -d ${D}/usr/lib/udrm
      for lib in libudrm.so; do
         install -m 0644 ${S}/src/.libs/${lib} -D ${D}/usr/lib/udrm/${lib}
   done
}
