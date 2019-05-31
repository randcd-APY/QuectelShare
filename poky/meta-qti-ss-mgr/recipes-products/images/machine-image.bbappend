#  Following is the order of priority for inclusion.
#  P1: <basemachine>/<basemachine>-<distro>-ss-mgr-image.inc
#  P2: <basemachine>/<basemachine>-ss-mgr-image.inc
#  P3: common/common-ss-mgr-image.inc
include ${@get_bblayer_img_inc('ss-mgr', d)}

