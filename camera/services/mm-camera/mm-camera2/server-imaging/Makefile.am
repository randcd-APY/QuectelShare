#
# mm-qcamera-daemon
#

MM_CAMERA_PATH = $(top_srcdir)
bindir=/system/bin

bin_PROGRAMS = mm-qcamera-daemon
mm_qcamera_daemon_CFLAGS  = -Werror

mm_qcamera_daemon_CPPFLAGS = -DMSM_CAMERA_BIONIC
mm_qcamera_daemon_CPPFLAGS += -DMEMLEAK_FLAG
mm_qcamera_daemon_CPPFLAGS += -DFDLEAK_FLAG

#mm_qcamera_daemon_CPPFLAGS += -include bionic/libc/kernel/common/linux/types.h
#mm_qcamera_daemon_CPPFLAGS += -include bionic/libc/kernel/common/linux/socket.h
#mm_qcamera_daemon_CPPFLAGS += -include bionic/libc/kernel/common/linux/in.h
#mm_qcamera_daemon_CPPFLAGS += -include bionic/libc/kernel/common/linux/un.h

mm_qcamera_daemon_SOURCES = server.c server_process.c

mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)
mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)/includes/
mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)/services/thread/include
mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/bus/
mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/controller/
mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/object/
mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/includes/
mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/tools/
mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/event/
mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/pipeline/
mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/stream/
mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/module/
mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/port/
mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/debug/
mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/debug/memleak/
mm_qcamera_daemon_CPPFLAGS += -I $(MM_CAMERA_PATH)/media-controller/mct/debug/fdleak

mm_qcamera_daemon_CPPFLAGS += -I $(CAMERAHAL_PATH)/QCamera2/stack/common
mm_qcamera_daemon_CPPFLAGS += -DDAEMON_PRESENT


mm_qcamera_daemon_LDADD = -lcutils -ldl ../media-controller/mct/libmmcamera2_mct.la \
                                   ../media-controller/modules/stats/libmmcamera2_stats_modules.la \
                                   ../media-controller/modules/iface2/libmmcamera2_iface_modules.la \
                                   ../media-controller/modules/isp2/module/libmmcamera2_isp_modules.la \
                                   ../media-controller/modules/sensors/libmmcamera2_sensor_modules.la \
                                   ../media-controller/modules/pproc-new/libmmcamera2_pproc_modules.la \
                                   ../media-controller/modules/imglib/libmmcamera2_imglib_modules.la \
                                   -lmmcamera_dbg

mm_qcamera_daemon_LDADD += ../services/thread/libmmcamera_thread_services.la

mm_qcamera_daemon_LDFLAGS = -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=realloc -Wl,--wrap=calloc
mm_qcamera_daemon_LDFLAGS += -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=socket -Wl,--wrap=pipe -Wl,--wrap=mmap
