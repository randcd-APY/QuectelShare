FILESEXTRAPATHS_prepend := "${THISDIR}/web-setup:"

SRC_URI_append = "\
    file://200-Make_UI_CGI_default_URL.patch;striplevel=2 \
    "
