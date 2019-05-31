SUMMARY = "Wayland Protocol upgrade recipe for wayland-1.9.0"
FILESEXTRAPATHS_prepend := "${WORKSPACE}/poky/meta-qti-display/recipes/wayland:"
SRC_URI_append = " \
          file://wayland-protocol-Add-wl_output-v3.patch \
          file://qti-patches/src-add-gbm-buffer-backend-header-file.patch \
       "

do_install_append_apq8098() {
       cp -rf ${S}/src/gbm-buffer-backend.h                               ${D}${includedir}
}