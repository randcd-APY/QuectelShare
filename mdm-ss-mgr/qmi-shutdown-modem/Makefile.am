AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -Dstrlcpy=g_strlcpy \
        -g -O0 \
        -fno-inline \
        -fno-short-enums \
        -fpic \
        $(QMIF_CFLAGS) \
        $(QMI_CFLAGS) \
        -I.

requiredlibs = $(QMIF_LIBS) $(QMI_LIBS)

if TARGET_APQ
AM_CPPFLAGS = -DENABLE_TARGET_APQ
endif

bin_PROGRAMS = qmi_shutdown_modem
qmi_shutdown_modem_SOURCES = qmi-shutdown-modem.c
qmi_shutdown_modem_LDADD = $(requiredlibs)
qmi_shutdown_modem_LDFLAGS = -lpthread -lqmiservices -ldl @GLIB_LIBS@
qmi_shutdown_modem_CC = @CC@

