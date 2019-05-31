
ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS = qti_system_daemon
qti_system_daemon_LDADD := -ldl ../libmmi/libmmi.la $(DIAG_LIBS) -lpthread -lm
qti_system_daemon_CPPFLAGS = -O2
qti_system_daemon_CPPFLAGS += -g
qti_system_daemon_CPPFLAGS += -Wno-all
qti_system_daemon_CPPFLAGS += -Wno-error
qti_system_daemon_CPPFLAGS += -Wno-unused-parameter
qti_system_daemon_CPPFLAGS += -I .
qti_system_daemon_CPPFLAGS += -I../libmmi
qti_system_daemon_CPPFLAGS += -I$(WORKSPACE)/system/core/include
qti_system_daemon_CPPFLAGS += -std=c++11 -DLINUX_ENABLED $(DIAG_CFLAGS)
if USE_GLIB
qti_system_daemon_CPPFLAGS += -DUSE_GLIB $(GLIB_CFLAGS)
qti_system_daemon_LDADD += $(GLIB_LIBS)
endif

if NO_UI
qti_system_daemon_CPPFLAGS +=  -DNO_UI
endif
if ANDROID
qti_system_daemon_CPPFLAGS +=  -DANDROID
endif
if USE_LIBMINIUI
qti_system_daemon_CPPFLAGS +=  -DUSE_LIBMINIUI
endif
# source to compile
qti_system_daemon_SOURCES = qti_system_daemon.cpp
