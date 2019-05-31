# Automake script for QBI

bin_PROGRAMS = mbimd

mbimd_CFLAGS = -Wall \
               -Wunused \
               -Wstrict-prototypes \
               -fno-short-enums

# Note that there are header files under qmi/ and qmi-framework/ with the
# same name, but exposing different APIs (e.g. qmi_client.h). We want the
# qmi-framework version for all conflicting files, so make sure it's first
mbimd_CPPFLAGS = $(MBIM_CFLAGS) \
                 -Iframework/inc \
                 -Iplatform/api \
                 -Iplatform/inc \
                 -Isvc/inc \
                 -I../wmsts/api \
                 -I../qmi_svc/api \
                 $(DIAG_CFLAGS) \
                 $(QMIF_CFLAGS) \
                 $(QMI_CFLAGS)

qbi_framework_dir = framework/src
qbi_framework_c_sources = \
   $(qbi_framework_dir)/qbi_mem.c \
   $(qbi_framework_dir)/qbi_msg.c \
   $(qbi_framework_dir)/qbi_qmi.c \
   $(qbi_framework_dir)/qbi_qmi_txn.c \
   $(qbi_framework_dir)/qbi_qmux.c \
   $(qbi_framework_dir)/qbi_svc.c \
   $(qbi_framework_dir)/qbi_task.c \
   $(qbi_framework_dir)/qbi_txn.c \
   $(qbi_framework_dir)/qbi_util.c

qbi_platform_dir = platform/src
qbi_platform_c_sources = \
   $(qbi_platform_dir)/qbi_diag_linux.c \
   $(qbi_platform_dir)/qbi_fwupd_linux.c \
   $(qbi_platform_dir)/qbi_hc_linux.c \
   $(qbi_platform_dir)/qbi_log_qxdm.c \
   $(qbi_platform_dir)/qbi_nv_store_linux.c \
   $(qbi_platform_dir)/qbi_os_linux.c \
   $(qbi_platform_dir)/qbi_qmux_smd_linux.c \
   $(qbi_platform_dir)/qbi_task_init_linux.c

qbi_svc_dir = svc/src
qbi_svc_c_sources = \
   $(qbi_svc_dir)/qbi_svc_atds.c \
   $(qbi_svc_dir)/qbi_svc_auth.c \
   $(qbi_svc_dir)/qbi_svc_bc.c \
   $(qbi_svc_dir)/qbi_svc_bc_ext.c \
   $(qbi_svc_dir)/qbi_svc_bc_ext_prov.c \
   $(qbi_svc_dir)/qbi_svc_bc_ext_lte.c \
   $(qbi_svc_dir)/qbi_svc_bc_ext_dssa.c \
   $(qbi_svc_dir)/qbi_svc_bc_spdp.c \
   $(qbi_svc_dir)/qbi_svc_bc_nas.c \
   $(qbi_svc_dir)/qbi_svc_bc_sim.c \
   $(qbi_svc_dir)/qbi_svc_ext_qmux.c \
   $(qbi_svc_dir)/qbi_svc_msfwid.c \
   $(qbi_svc_dir)/qbi_svc_mshsd.c \
   $(qbi_svc_dir)/qbi_svc_msuicc.c \
   $(qbi_svc_dir)/qbi_svc_pb.c \
   $(qbi_svc_dir)/qbi_svc_qmbe.c \
   $(qbi_svc_dir)/qbi_svc_qdu.c \
   $(qbi_svc_dir)/qbi_svc_sms.c \
   $(qbi_svc_dir)/qbi_svc_sms_pdu.c \
   $(qbi_svc_dir)/qbi_svc_stk.c \
   $(qbi_svc_dir)/qbi_svc_ussd.c \
   $(qbi_svc_dir)/qbi_svc_mssar.c

mbimd_SOURCES = $(qbi_framework_c_sources) $(qbi_platform_c_sources) $(qbi_svc_c_sources)
mbimd_LDADD = -lcutils -lpthread -lrt ../wmsts/libmbim_wmsts.la ../qmi_svc/libmbim_qmi_svc.la $(DIAG_LIBS) $(QMIF_LIBS)

# Development only below, should be commented out for release
#mbimd_CPPFLAGS += -DFEATURE_QBI_VERBOSE_KMSG_LOGGING
#mbimd_CPPFLAGS += -g -Itest/inc -DQBI_UNIT_TEST
#mbimd_SOURCES += test/src/qbi_ut.c test/src/qbi_utf.c test/src/qbi_utf_shared.c test/src/qbi_utf_qdu_linux.c
