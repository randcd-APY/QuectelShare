DEPENDS = "libnl diag glib-2.0 system-core ath6kl-utils"
EXTRA_OECONF = " \
                --with-glib \
                --enable-wlan=yes \
                --enable-debug=yes \
                "

EXTRA_OECONF += "--enable-target=${BASEMACHINE}"

do_configure_prepend () {
    sed -i -e 's/$(LIBNL_LIBS)/$(LIBNL_LIBS) -lrt/g' ${S}/Makefile.am
    sed -i -e 's/$(requiredlibs) -lbtnv/$(requiredlibs)/g' ${S}/Makefile.am
}

