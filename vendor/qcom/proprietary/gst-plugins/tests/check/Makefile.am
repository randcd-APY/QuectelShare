#Portions of this file are copied from GStreamer template which GStreamer provides under the MIT license.

include $(top_srcdir)/common/check.mak

CHECK_REGISTRY = $(top_builddir)/tests/check/test-registry.reg
TEST_FILES_DIRECTORY = $(top_srcdir)/tests/files

GST_TOOLS_DIR = $(top_builddir)/tools

REGISTRY_ENVIRONMENT = \
	GST_REGISTRY_1_0=$(CHECK_REGISTRY)

# GST_PLUGINS_XYZ_DIR is only set in an uninstalled setup
AM_TESTS_ENVIRONMENT += \
	$(REGISTRY_ENVIRONMENT)                                 \
	GST_PLUGIN_SYSTEM_PATH_1_0=				\
	GST_PLUGIN_PATH_1_0=$(top_builddir)/gst:$(top_builddir)/sys:$(top_builddir)/ext:$(GST_PLUGINS_LIBAV_DIR):$(GST_PLUGINS_UGLY_DIR):$(GST_PLUGINS_GOOD_DIR):$(GST_PLUGINS_BASE_DIR):$(GST_PLUGINS_DIR) \
	GST_PLUGIN_LOADING_WHITELIST="gstreamer@$(GST_PLUGINS_DIR):gst-plugins-base@$(GSTPB_PLUGINS_DIR):gst-plugins-good:gst-plugins-ugly:gst-libav:gst-plugins-bad@$(top_builddir)" \
	GST_STATE_IGNORE_ELEMENTS=""

plugindir = $(libdir)/gstreamer-@GST_API_VERSION@

# override to _not_ install the test plugins
install-pluginLTLIBRARIES:

# the core dumps of some machines have PIDs appended
CLEANFILES = core.* test-registry.*

SUPPRESSIONS = $(top_srcdir)/common/gst.supp $(srcdir)/gst-plugins-qti.supp

clean-local: clean-local-check
distclean-local: distclean-local-orc

distclean-local-orc:
	rm -rf orc

VALGRIND_TO_FIX = 

# valgrind testing
VALGRIND_TESTS_DISABLE = \
	$(VALGRIND_TO_FIX)

# these tests don't even pass
noinst_PROGRAMS = 

check_PROGRAMS = \
	$(EXPERIMENTAL_CHECKS)

noinst_HEADERS = 

TESTS = $(check_PROGRAMS)

AM_CFLAGS = $(GST_CFLAGS) $(GST_CHECK_CFLAGS) $(GST_OPTION_CFLAGS) \
	-DGST_CHECK_TEST_ENVIRONMENT_BEACON="\"GST_PLUGIN_LOADING_WHITELIST\"" \
	-DGST_TEST_FILES_PATH="\"$(TEST_FILES_DIRECTORY)\"" \
	-UG_DISABLE_ASSERT -UG_DISABLE_CAST_CHECKS $(PTHREAD_CFLAGS)
LDADD = $(GST_CHECK_LIBS)

EXTRA_DIST = gst-plugins-qti.supp


