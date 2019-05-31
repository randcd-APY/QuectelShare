EXTRAPATHS_prepend_apq8098 := "${WORKSPACE}/vendor/qcom/proprietary/gst-media/display:"

do_configure_prepend_apq8098 () {
   if [ ${@os.path.isfile('${WORKSPACE}/vendor/qcom/proprietary/gst-media/display/ionbuf/src/gstionbuf_meta.h')} == "True" ]; then
      cp -rf ${WORKSPACE}/vendor/qcom/proprietary/gst-media/display/ionbuf/src ${S}/gst-libs/gst/ionbuf/
      cp ${WORKSPACE}/vendor/qcom/proprietary/gst-media/display/ionbuf/pkgconfig/gstreamer-ionbuf.pc.in ${S}/pkgconfig
      cp ${WORKSPACE}/vendor/qcom/proprietary/gst-media/display/ionbuf/pkgconfig/gstreamer-ionbuf-uninstalled.pc.in ${S}/pkgconfig
   else
      install -d ${S}/gst-libs/gst/ionbuf/
      echo -n "" > ${S}/gst-libs/gst/ionbuf/Makefile.am
   fi

	install -d ${STAGING_DIR_HOST}${datadir}/wayland-protocols/stable/gbm-buffer-backend/
	cp ${WORKSPACE}/display/weston/protocol/gbm-buffer-backend.xml ${STAGING_DIR_HOST}${datadir}/wayland-protocols/stable/gbm-buffer-backend
}

EXTRA_OECONF_append_apq8098 = "${@" HAVE_GSTIONBUF_META_H=yes" if os.path.isfile('${WORKSPACE}/vendor/qcom/proprietary/gst-media/display/ionbuf/src/gstionbuf_meta.h') == True else " HAVE_GSTIONBUF_META_H=no"}"
