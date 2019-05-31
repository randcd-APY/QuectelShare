#Portions of this file are copied from GStreamer template which GStreamer provides under the MIT license.

if HAVE_GST_CHECK
SUBDIRS_CHECK = check
else
SUBDIRS_CHECK =
endif

if BUILD_EXAMPLES
SUBDIRS_EXAMPLES = examples
else
SUBDIRS_EXAMPLES =
endif

SUBDIRS = $(SUBDIRS_CHECK) $(SUBDIRS_EXAMPLES) files

DIST_SUBDIRS = check examples files
