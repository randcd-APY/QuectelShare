AM_CFLAGS = -Wall -g \
         -Wundef \
         -Wstrict-prototypes\
         -Wno-trigraphs

AM_CPPFLAGS = -D__packed__= \
              -I$(WORKSPACE)/security/securemsm/sampleclient/ \
              -I$(WORKSPACE)/security/securemsm-noship/QSEEComAPI/ \
              -I$(WORKSPACE)/security/securemsm/QSEEComAPI/ \
              -DFEATURE_LE_DIAG

lib_LTLIBRARIES = libgstcrypto.la

libgstcrypto_la_SOURCES = gst_crypto.c


libgstcrypto_la_CFLAGS = $(AM_CFLAGS)
libgstcrypto_la_CPPFLAGS = $(AM_CPPFLAGS)
libgstcrypto_la_LDFLAGS = -lpthread -lutils -ldl -lcutils -llog -lQseeComApi

libgstcrypto_la_LDFLAGS += -shared -avoid-version

