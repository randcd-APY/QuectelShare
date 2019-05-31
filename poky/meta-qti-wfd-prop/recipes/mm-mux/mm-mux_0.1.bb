inherit autotools deploy pkgconfig qlicense qprebuilt qcommon sdllvm

SUMMARY = "mm-mux"
SECTION = "multimedia"
DESCRIPTION = "mm-mux"

PR = "r4"
SRC_DIR = "${WORKSPACE}/video/lib/mm-mux/"

FILESPATH =+ "${WORKSPACE}:"
SRC_URI = "file://video/lib/mm-mux/"

SRCREV = "${AUTOREV}"
S      = "${WORKDIR}/video/lib/mm-mux"

PACKAGES = "${PN}-dbg ${PN} ${PN}-dev"

DEPENDS += "glib-2.0"
DEPENDS += "mm-osal"
DEPENDS += "mm-parser"
DEPENDS += "mm-parser-noship"
DEPENDS += "media"
DEPENDS += "common"

RDEPENDS_${PN} += "mm-osal media mm-parser-noship"

EXTRA_OECONF_append =" --with-utils-headers=${STAGING_INCDIR}/utils/"
EXTRA_OECONF_append =" --with-cutils-headers=${STAGING_INCDIR}/cutils/"
EXTRA_OECONF_append =" --with-mmparser-headers=${STAGING_INCDIR}/mm-parser/include/"
EXTRA_OECONF_append =" --with-mmosal-headers=${STAGING_INCDIR}/mm-osal/include/"
EXTRA_OECONF_append =" --with-mmcore-headers=${STAGING_INCDIR}/mm-core/"

FILES_${PN}-dbg = "${libdir}/.debug ${bindir}/.debug"
FILES_${PN}     = "${bindir}/* ${libdir}/lib*.so*"
FILES_${PN} += "/etc /system"
FILES_${PN}-dev = "${libdir}/*.la  ${libdir}/lib*.so ${includedir} ${libdir}/pkgconfig"

do_install_append() {
install -d ${D}${includedir}/mm-mux/
install -m 0644 ${S}/main/MuxBaseLib/inc/qmmList.h -D ${D}${includedir}/mm-mux/qmmList.h
install -m 0644 ${S}/main/MuxBaseLib/inc/muxbase.h -D ${D}${includedir}/mm-mux/qmmList.h
install -m 0644 ${S}/main/MuxBaseLib/inc/isucceedfail.h -D ${D}${includedir}/mm-mux/qmmList.h
install -m 0644 ${S}/main/MuxBaseLib/inc/filesourcestring.h -D ${D}${includedir}/mm-mux/qmmList.h
install -m 0644 ${S}/main/MuxBaseLib/inc/oscl_file_io.h -D ${D}${includedir}/mm-mux/qmmList.h
install -m 0644 ${S}/main/MuxBaseLib/inc/zrex_string.h -D ${D}${includedir}/mm-mux/qmmList.h
install -m 0644 ${S}/main/FilemuxInternalDefs/inc/filemuxinternaldefs.h -D ${D}${includedir}/mm-mux/qmmList.h
install -m 0644 ${S}/main/FileMuxLib/inc/filemux.h -D ${D}${includedir}/mm-mux/qmmList.h
install -m 0644 ${S}/main/FileMuxLib/inc/filemuxtypes.h -D ${D}${includedir}/mm-mux/qmmList.h
install -m 0644 ${S}/main/FileMuxLib/inc/MuxQueue.h -D ${D}${includedir}/mm-mux/qmmList.h
install -m 0644 ${S}/main/MP2BaseFileLib/inc/MP2BaseFile.h -D ${D}${includedir}/mm-mux/qmmList.h
install -m 0644 ${S}/main/MP2BaseFileLib/inc/MP2StreamMuxConstants.h -D ${D}${includedir}/mm-mux/qmmList.h
install -d ${D}/system/etc/mm-mux
install -m 0644 ${S}/mux-test/mux-app/ConfigFile.xml -D ${D}/system/etc/mm-mux/ConfigFile.xml
}
INSANE_SKIP_${PN} += "dev-so"
EXCLUDE_FROM_SHLIBS = "1"
