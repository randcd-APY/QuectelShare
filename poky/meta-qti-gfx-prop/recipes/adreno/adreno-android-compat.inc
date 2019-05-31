inherit androidmk qsigning

FILESPATH =+ "${WORKSPACE}:"
SRC_URI  = "file://adreno200/"

SRCREV = "${AUTOREV}"

DEPENDS += "native-frameworks"
DEPENDS += "libxml2"
DEPENDS += "display-hal"

#use the perl-native from the yocto devtools
EXTRANATIVEPATH += "perl-native"

export TARGET_LIBRARY_SUPPRESS_LIST="libstlport libui libz libsync"

EXTRA_OEMAKE += "ANDROID_COMPAT=1"
export SECIMAGE_BASE= "${SIGNING_TOOLS_DIR}/SecImage"
export PILSPLITTER_BASE = "${SIGNING_TOOLS_DIR}"

CFLAGS += "-I${STAGING_INCDIR}/hardware/"
CFLAGS += "-I${STAGING_INCDIR}/cutils/"
CFLAGS += "-I${STAGING_INCDIR}/log/"
CFLAGS += "-I${STAGING_INCDIR}/glib-2.0"
CFLAGS += "-I${STAGING_LIBDIR}/glib-2.0/include"

CFLAGS += "-include string.h"

CFLAGS += "-D_GNU_SOURCE"
CFLAGS += "-D_LINUX_QC"
CFLAGS += "-fsigned-char"
CFLAGS += "-Wno-attributes"

LDFLAGS += "-lcutils"
LDFLAGS += "-llog"
LDFLAGS += "-lsync"
LDFLAGS += "-lxml2"
LDFLAGS += "-lz"

#Export framework opengl header path
export FRAMEWORK_OPENGL_INCLUDES="${STAGING_INCDIR}/opengl/include"

FILES_${PN} += "/lib/firmware/*"
FILES_${PN} += "/lib64/firmware/*"

INSANE_SKIP_${PN} += "already-stripped"
INSANE_SKIP_${PN} += "arch"

do_fixup_before_compile () {
    # comment out all the occurrences of "include \$(ADRENO_STL_INCLUDE)" in mk files
    find ${S}/ -type f -name "*.mk" -exec sed -i 's/include \$(ADRENO_STL_INCLUDE)/\#include \$(ADRENO_STL_INCLUDE)/g' {} +
}
addtask fixup_before_compile after do_patch before do_configure

do_install_append () {

  export FIRMWARE_DIR=${LA_OUT_DIR}/target/product/${LA_TARGET_PRODUCT}/system/etc/firmware

  install -d ${D}/lib/firmware
  install -d ${D}/lib64/firmware

  if [ "${MLPREFIX}" == "lib32-" ]; then
    export FIRMWARE_INSTALL_DIR=${D}/lib/firmware
  else
    export FIRMWARE_INSTALL_DIR=${D}/lib64/firmware
  fi

  for f in $(ls ${FIRMWARE_DIR}/*.fw* ); do
    install -m 0644 ${f} -D ${FIRMWARE_INSTALL_DIR}
  done

  for f in $(ls ${FIRMWARE_DIR}/*_zap.* ); do
    install -m 0644 ${f} -D ${FIRMWARE_INSTALL_DIR}
  done

  # Move the lib from /usr/lib/egl to /usr/lib
  mv ${D}${libdir}/egl/*.so ${D}${libdir}
  rm -rf ${D}${libdir}/egl

  # lose the files installed under /system, as most of the necessary
  # files are under system folder are already sent to lib/firmware
  rm -rf ${D}/system
}
