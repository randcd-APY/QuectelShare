# ss-mgr PROP PACKAGES
#  Following is the order of priority for inclusion.
#  P1: <basemachine>/<basemachine>-<distro>-ss-mgr-prop-image.inc
#  P2: <basemachine>/<basemachine>-ss-mgr--prop-image.inc
#  P3: common/common-ss-mgr-prop-image.inc
include ${@get_bblayer_img_inc('ss-mgr-prop', d)}

