# Additional AllPlay  packages to be put to the image filesystem.
include ${BASEMACHINE}/${BASEMACHINE}-allplay-image.inc

# THISDIR is only correct when parsing the recipe, not when executing,
# so we need to save it in a variable using ":="
SYMBOL_SCRIPT := "${THISDIR}/files/package_upload_symbols.sh"

do_symbols_archive() {
	"${SYMBOL_SCRIPT}" \
		--staging "${SDK_OUTPUT}${SDKTARGETSYSROOT}" \
		--bindir "${DEPLOY_DIR_IMAGE}" \
		--version $(cat "${IMAGE_ROOTFS}/etc/version")
}

# sdk for symbols, rootfs for version (technically not required since
# sdk also depends on rootfs)
addtask symbols_archive after do_populate_sdk do_rootfs
