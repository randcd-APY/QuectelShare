DEPENDS += "gobject-introspection gobject-introspection-native"
DEPENDS_append_apq8017 += "ffmpeg"
DEPENDS_append_msm8909 += "ffmpeg"

FILESEXTRAPATHS_prepend := "${THISDIR}/gstreamer1.0-libav:"

SRC_URI += "\
            file://0001-Expose-support-for-DSD.patch \
            file://0002-avdemux-Expose-IFF-container-support.patch \
            "

LICENSE_FLAGS_WHITELIST = "commercial"

###### GROUPS FOR LIBAV PLUGIN ######

AUDIO_PARSE_ENABLE_LIBAV = " --enable-demuxer=ape \
                             --enable-demuxer=iff  \
                             --enable-demuxer=dsf "

AUDIO_DECODE_ENABLE_LIBAV = " --enable-decoder=ape \
                              --enable-decoder=mp3 \
                              --enable-decoder=flac \
                              --enable-decoder=aac  \
                              --enable-decoder=alac \
                              --enable-decoder=wmalossless \
                              --enable-decoder=wmapro  \
                              --enable-decoder=wmav1  \
                              --enable-decoder=wmav2 \
                              --enable-decoder=wmavoice  \
                              --enable-decoder=dsd_lsbf \
                              --enable-decoder=dsd_lsbf_planar \
                              --enable-decoder=dsd_msbf \
                              --enable-decoder=dsd_msbf_planar "

AUDIO_ENCODE_ENABLE_LIBAV = " --enable-encoder=aac "

DISABLE_ALL_LIBAV = "--disable-everything "

CONFIGURE_PLUGINS_LIBAV = " --with-libav-extra-configure="${DISABLE_ALL_LIBAV} \
                                                          ${AUDIO_PARSE_ENABLE_LIBAV} \
                                                          ${AUDIO_DECODE_ENABLE_LIBAV} \
                                                          ${AUDIO_ENCODE_ENABLE_LIBAV} " \
                          "

EXTRA_OECONF_append_qcs403-som2 = "${CONFIGURE_PLUGINS_LIBAV}"
EXTRA_OECONF_append_qcs405-som1 = "${CONFIGURE_PLUGINS_LIBAV}"
