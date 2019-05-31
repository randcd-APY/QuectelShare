inherit qlicense

BBCLASSEXTEND =+ "native"

# keep this in-sync with qsigning.bbclass
SIGNING_TOOLS_DIR = "${TMPDIR}/work-shared/signing_tools/SecImage"

DESCRIPTION = "Sectool5: Image signing tools"
PR = "r1"

FILESPATH =+ "${WORKSPACE}/vendor/qcom/proprietary/:"
SRC_URI = "file://sectools"

S = "${WORKDIR}/sectools"

do_unpack[cleandirs] += " ${S} ${SIGNING_TOOLS_DIR}"
do_clean[cleandirs] += " ${S} ${SIGNING_TOOLS_DIR}"

# Move the scripts to a work-shared directory as described by SIGNING_TOOLS_DIR
#
base_do_unpack_append () {
    s = d.getVar("S", True)
    if s[-1] == '/':
        # drop trailing slash, so that os.symlink(signing_dir, s) doesn't use s as directory name and fail
        s=s[:-1]
    signing_dir = d.getVar("SIGNING_TOOLS_DIR", True)
    if s != signing_dir:
        bb.utils.mkdirhier(signing_dir)
        bb.utils.remove(signing_dir, recurse=True)
        import shutil
        shutil.move(s, signing_dir)
        os.symlink(signing_dir, s)
}

# don't run these functions
#
do_configure[noexec] = "1"
do_compile[noexec] = "1"
do_install[noexec] = "1"
