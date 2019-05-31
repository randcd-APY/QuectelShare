#Portions of this file are copied from GStreamer template which GStreamer provides under the MIT license.

DISTCHECK_CONFIGURE_FLAGS=--host=arm-oe-linux-gnueabi

SUBDIRS = \
        sys pkgconfig \
        bt-helper pkgconfig \
        apps pkgconfig \
        m4 common tests data gst

DIST_SUBDIRS = \
        sys pkgconfig \
        bt-helper pkgconfig \
        apps pkgconfig \
        m4 common tests data gst

if ENABLE_OMX
 SUBDIRS += omx pkgconfig
 DIST_SUBDIRS += omx pkgconfig
endif

EXTRA_DIST = \
	depcomp \
	AUTHORS COPYING NEWS README RELEASE REQUIREMENTS \
	ChangeLog gst-plugins-qti.doap autogen.sh

DISTCLEANFILES = arm-oe-linux-gnueabi-libtool

ACLOCAL_AMFLAGS = -I m4 -I common/m4

include $(top_srcdir)/common/release.mak
include $(top_srcdir)/common/po.mak

check-valgrind:
	$(MAKE) -C tests/check check-valgrind

if HAVE_GST_CHECK
check-torture:
	$(MAKE) -C tests/check torture
build-checks:
	$(MAKE) -C tests/check build-checks
else
check-torture:
	true
build-checks:
	true
endif

include $(top_srcdir)/common/coverage/lcov.mak

# cruft: plugins that have been merged or moved or renamed

CRUFT_FILES =

CRUFT_DIRS =

include $(top_srcdir)/common/cruft.mak

all-local: check-cruft
