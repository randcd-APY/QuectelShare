inherit autotools qcommon qprebuilt qlicense

DESCRIPTION = "Time Genoff Library"
PR = "r0"

SRC_DIR = "${WORKSPACE}/time-services/time-genoff"
FILESPATH =+ "${WORKSPACE}/time-services:"
SRC_URI = "file://time-genoff"

S = "${WORKDIR}/time-genoff"
