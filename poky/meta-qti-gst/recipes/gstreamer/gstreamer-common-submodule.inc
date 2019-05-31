#common project used in plugin bb.

do_patch_append() {
    bb.build.exec_func('do_clone_gst_common', d)
}

do_clone_gst_common () {
    cd ${S}
    #rm -rf common
    #touch NEWS && touch AUTHORS && touch ChangeLog && touch README && touch REQUIREMENTS
    #git clone -b gstreamer/common/master https://source.codeaurora.org/quic/le/gstreamer/common
}
