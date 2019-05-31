inherit autotools qcommon qlicense qprebuilt
DESCRIPTION = "ffmpeg video"
PR = "r1"

FILESPATH =+ "${WORKSPACE}:"

SRC_URI = "file://ffmpeg_video"

SRC_DIR = "${WORKSPACE}/ffmpeg_video"

S = "${WORKDIR}/ffmpeg_video"

DEPENDS := "ffmpeg"

EXTRA_OECONF_append = " --with-glib"
EXTRA_OECONF_append = " --with-sanitized-headers=${STAGING_KERNEL_BUILDDIR}/usr/include"

do_install() {
	install -m 0755 ${S}/encode_video -D ${D}/bin/encode_video
	install -m 0755 ${S}/decode_video -D ${D}/bin/decode_video
}

do_configure[depends] += "virtual/kernel:do_shared_workdir"
