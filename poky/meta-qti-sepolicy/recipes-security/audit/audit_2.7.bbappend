EXTRA_OECONF += "--without-prelude \
        --with-libwrap \
        --enable-gssapi-krb5=no \
        --with-libcap-ng=no \
        --with-python=no \
        --libdir=${base_libdir} \
        --sbindir=${base_sbindir} \
        --without-python3 \
        --disable-zos-remote \
        "
