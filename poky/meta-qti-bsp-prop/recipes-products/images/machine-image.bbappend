#  Following is the order of priority for inclusion.
#  P1: <basemachine>/<basemachine>-<distro>-qti-image.inc
#  P2: <basemachine>/<basemachine>-qti-image.inc
#  P3: common/common-qti-image.inc
include ${@get_bblayer_img_inc('qti', d)}

require internal-image.inc
# Set up for handling the generation of the /usr image
# partition...
require mdm-usr-image.inc

# Set up for handling the generation of the /cache image
# partition...
require mdm-cache-image.inc

# Set up for handling the generation of the /persist image
# partition only for APQ Targets
require apq-persist-image.inc

do_rootfs[nostamp] = "1"
do_build[nostamp]  = "1"

