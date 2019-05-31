# mm-qcamera-daemon

MM2_PATH = $(top_srcdir)/mm-camera2
BUILD_PATH = $(top_srcdir)/../../../build/mm-camera2
bindir=/system/bin

bin_PROGRAMS = mm-qcamera-daemon

mm_qcamera_daemon_CFLAGS = $(DEBUG_CFLAGS)
mm_qcamera_daemon_CFLAGS += -DAMSS_VERSION=$(AMSS_VERSION)
mm_qcamera_daemon_CFLAGS += -D_ANDROID_
mm_qcamera_daemon_CFLAGS += -DMSM_CAMERA_GCC
mm_qcamera_daemon_CFLAGS += -DMSM_CAMERA_BIONIC
mm_qcamera_daemon_CFLAGS += -DLOGE=ALOGE
mm_qcamera_daemon_CFLAGS += -include stdint.h
mm_qcamera_daemon_CFLAGS += -DDAEMON_PRESENT

mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/modules
mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/server-imaging
mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/includes
mm_qcamera_daemon_CFLAGS += -I $(WORKSPACE)/camera/services/camera-lib-legacy/QCamera2/stack/common/
mm_qcamera_daemon_CFLAGS += -I $(WORKSPACE)/camera/lib-legacy/QCamera2/stack/common/
mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/mct/bus/
mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/mct/controller/
mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/mct/object/
mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/includes/
mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tools/
mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/mct/event/
mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/mct/pipeline/
mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/mct/stream/
mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/mct/module/
mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/mct/port/
mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/mct/tuning/
mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/mct/debug/

	ifeq ($(KERNEL_VER), 4.9%)
mm_qcamera_daemon_CFLAGS += -I $(WORKSPACE)/kernel/msm-4.9/include/
	else
mm_qcamera_daemon_CFLAGS += -I $(WORKSPACE)/kernel/msm-3.18/include/
	endif

#mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/modules/stats
#mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/is
#mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/q3a
#mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/q3a/aec
#mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/q3a/af
#mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/q3a/awb
#mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/asd
#mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/afd
#mm_qcamera_daemon_CFLAGS += -I $(MM2_PATH)/media-controller/modules/stats/debug-data


#sources
mm_qcamera_daemon_SOURCES = server.c
mm_qcamera_daemon_SOURCES += server_process.c

mm_qcamera_daemon_LDADD = ../media-controller/mct/liboemcamera.la
mm_qcamera_daemon_LDADD += ../media-controller/modules/stats/libmmcamera2_stats_modules.la
mm_qcamera_daemon_LDADD += ../media-controller/modules/iface/libmmcamera2_iface_modules.la
mm_qcamera_daemon_LDADD += ../media-controller/modules/isp/libmmcamera2_isp_modules.la
mm_qcamera_daemon_LDADD += ../media-controller/modules/sensors/libmmcamera2_sensor_modules.la
mm_qcamera_daemon_LDADD += ../media-controller/modules/pproc-new/libmmcamera2_pproc_modules.la
mm_qcamera_daemon_LDADD += ../media-controller/modules/imglib/libmmcamera2_imglib_modules.la
#mm_qcamera_daemon_LDADD += -ldl -lcutils -lrt -lm -loemcamera -lmmcamera2
mm_qcamera_daemon_LDADD += -ldl -llog -lcutils -lmmcamera2_stats_modules -lmmcamera2_is -lmmcamera2_pproc_modules -lmmcamera2_imglib_modules
mm_qcamera_daemon_LDADD += $(BUILD_PATH)/media-controller/mct/liboemcamera.la

