inherit qlicense

BBCLASSEXTEND =+ "native"

DESCRIPTION = "avbtool: Image signing tool"
PR = "r1"

FILESPATH =+ "${WORKSPACE}/android_compat/common/scripts:"
SRC_URI = "file://avbtool"

do_install(){
    install -d ${TMPDIR}/work-shared/avbtool/
    cp -r ${WORKDIR}/avbtool ${TMPDIR}/work-shared/avbtool/
}

#don't run these functions
do_configure[noexec] = "1"
do_compile[noexec] = "1"
