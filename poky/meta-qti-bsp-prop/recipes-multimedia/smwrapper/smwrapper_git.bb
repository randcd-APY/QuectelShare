inherit qcommon qprebuilt qlicense

DESCRIPTION = "smwrapper Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/audio/mm-audio/smwrapper/"
S = "${WORKDIR}/audio/mm-audio/smwrapper/"

def get_depends(d):
    if d.getVar('QSTHW_API_COND', 'false') == 'true':
        return "libcutils system-media qsthw-api"
    else:
        return "libcutils system-media"

QSTHW_API_COND = "true"
DEPENDS = "${@get_depends(d)}"

EXTRA_OEMAKE += "DEFAULT_INCLUDES="-I${STAGING_INCDIR}/mm-audio/qsthw_api""
EXTRA_OECONF += "BOARD_SUPPORTS_QSTHW_API=true"

FILES_${PN}-dbg  = "${libdir}/.debug/*"
FILES_${PN}      = "${libdir}/*.so ${libdir}/*.so.* ${sysconfdir}/* ${libdir}/pkgconfig/*"
FILES_${PN}-dev  = "${libdir}/*.la ${includedir}"
