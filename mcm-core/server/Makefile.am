AM_CFLAGS = -Wall \
        -Wundef \
        -Wstrict-prototypes \
        -fno-inline \
        -fno-short-enums \
        -fpic -g

ACLOCAL_AMFLAGS = -I m4
AM_CPPFLAGS = -D__packed__= \
        -DFEATURE_LINUX \
        -DFEATURE_NATIVELINUX \
	-DMULTI_PROCESS_CONFIG \
        $(QMIFRAMEWORK_CFLAGS) \
        $(LIBALSAINTF_CFLAGS) \
        $(QMI_CFLAGS) \
        $(DIAG_FLAGS) \
        -I./../ipc/ \
        -I./ \
        -I./../../../diag/include/ \
        -I./core/core/ \
        -I./cri/core/ \
        -I./cri/nas \
        -I./cri/dms \
        -I./cri/wms \
        -I./cri/uim \
        -I./cri/voice \
        -I./cri/data \
        -I./hlos/core/ \
        -I./hlos/dms/ \
        -I./hlos/nas/ \
        -I./hlos/sms/ \
        -I./hlos/voice/ \
        -I./hlos/uim/ \
        -I./hlos/srv_mgr/ \
        -I./utilities/bit_field/ \
        -I./utilities/control/ \
        -I./utilities/list/ \
        -I./utilities/log/ \
        -I./utilities/memory/ \
        -I./utilities/synchronization/ \
        -I./utilities/timer/ \
        -I./utilities/htable/ \
        -I./utilities/ \
        -I./../mcm_ssr_util

c_sources =     ../ipc/mcm_dm_v01.c \
                ../ipc/mcm_nw_v01.c \
                ../ipc/mcm_voice_v01.c \
                ../ipc/mcm_sms_v01.c \
                ../ipc/mcm_common_v01.c \
                ../ipc/mcm_ril_v01.c \
                ../ipc/mcm_sim_v01.c \
                ../ipc/mcm_client_v01.c \
                ../ipc/mcm_data_srv_mgr_v01.c \
                ../ipc/mcm_atcop_srv_mgr_v01.c \
                ../ipc/mcm_mobileap_srv_mgr_v01.c \
                ../ipc/mcm_loc_srv_mgr_v01.c \
                ../ipc/mcm_ipc.c \
                core/core/core_handler.c \
                core/core/core_event_lookup.c \
                core/core/core_queue_util.c \
                core/core/core_flow_control.c \
                cri/core/cri_core.c \
                cri/core/cri_event_lookup.c \
                cri/core/cri_rule_handler.c \
                cri/nas/cri_nas.c \
                cri/nas/cri_nas_core.c \
                cri/nas/cri_nas_utils.c \
                cri/nas/cri_nas_rules.c \
                cri/dms/cri_dms.c \
                cri/dms/cri_dms_core.c \
                cri/dms/cri_dms_utils.c \
                cri/dms/cri_dms_rules.c \
                cri/uim/cri_uim_core.c \
                cri/uim/cri_uim_utils.c \
                cri/wms/cri_wms_core.c \
                cri/wms/cri_wms.c \
                cri/voice/cri_voice.c \
                cri/voice/cri_voice_answer.c \
                cri/voice/cri_voice_call_info.c \
                cri/voice/cri_voice_call_list.c \
                cri/voice/cri_voice_call_obj.c \
                cri/voice/cri_voice_call_summary.c \
                cri/voice/cri_voice_core.c \
                cri/voice/cri_voice_dial.c \
                cri/voice/cri_voice_get_calls.c \
                cri/voice/cri_voice_hangup.c \
                cri/voice/cri_voice_ind_hdlr.c \
                cri/voice/cri_voice_multi_calls.c \
                cri/voice/cri_voice_qmi_client.c \
                cri/voice/cri_voice_settings.c \
                cri/voice/cri_voice_utils.c \
                cri/voice/cri_voice_cache.c \
                cri/voice/cri_voice_dtmf.c \
                cri/voice/cri_voice_ss.c \
                cri/voice/cri_common_dial_core.c \
                cri/voice/cri_common_dial_v01.c \
                cri/voice/cri_voice_update_ecall_msd.c \
                cri/data/cri_data.c \
                cri/data/cri_data_core.c \
                hlos/core/mcm_srv_multi_process.c \
                hlos/core/mcm_srv_internal.c \
                hlos/core/mcm_srv_ext.c \
                hlos/core/mcm_srv_adaptor.c \
                hlos/core/mcm_msg_queue.c \
                hlos/core/mcm_srv_audio.c \
                hlos/core/hlos_event_lookup.c \
                hlos/core/hlos_core.c \
                hlos/core/hlos_flow_control.c \
                hlos/dms/hlos_dms_core.c \
                hlos/nas/hlos_nas_core.c \
                hlos/sms/hlos_sms_core.c \
                hlos/voice/hlos_voice_core.c \
                hlos/uim/mcm_uim_request.c \
                hlos/uim/mcm_uim_response.c \
                hlos/uim/mcm_uim_util.c \
                hlos/uim/mcm_uim_indication.c \
                hlos/srv_mgr/hlos_srv_mgr.c \
                utilities/bit_field/util_bit_field.c \
                utilities/list/util_list.c \
                utilities/log/util_log.c \
                utilities/control/control_core.c \
                utilities/memory/util_memory.c        \
                utilities/synchronization/util_synchronization.c \
                utilities/timer/util_timer.c \
                utilities/timer/timer_event_lookup.c \
                utilities/htable/util_htable.c


bin_PROGRAMS = mcm_ril_service
if USE_AUDIO
lib_LTLIBRARIES = libmcm_srv_audio.la
endif

init_ddir = ${sysconfdir}/init.d
init_d_SCRIPTS = start_mcm_ril_srv_le

mcm_ril_service_CC = @CC@
mcm_ril_service_SOURCES = $(c_sources)
mcm_ril_service_LDADD = -lpthread -lm $(QMIFRAMEWORK_LIBS) -lqmiservices ../mcm_ssr_util/libmcm_ssr_util.la -ldl $(DIAG_LIBS) @GLIB_LIBS@
if USE_AUDIO
mcm_ril_service_LDFLAGS = -rdynamic
endif
mcm_ril_service_CPPFLAGS = -Dstrlcat=g_strlcat -Dstrlcpy=g_strlcpy $(AM_CPPFLAGS)
pkgconfigdir = $(libdir)/pkgconfig

if USE_AUDIO
libmcm_srv_audio_la_SOURCES = hlos/core/mcm_srv_audio.c
libmcm_srv_audio_la_CPPFLAGS = -Dstrlcat=g_strlcat -Dstrlcpy=g_strlcpy -DMCM_AUDIO_SHARED_LIBRARY $(AM_CPPFLAGS)
libmcm_srv_audio_la_LIBADD = -lpthread -lm -lalsa_intf_msm8909 $(DIAG_FLAGS) @GLIB_LIBS@
endif
EXTRA_DIST = $(pkgconfig_DATA)
