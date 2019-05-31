# common signing variables to assist with signing a loadable image.
# these tools complement the boot-loader's and peripheral image loader's
# verification stage.
#

export SIGNING_TOOLS_DIR = "${TMPDIR}/work-shared/signing_tools"

DEPENDS += "sectool5-native openssl-native python-native"
