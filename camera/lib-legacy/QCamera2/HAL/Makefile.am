ACLOCAL_AMFLAGS = -I m4
curr_path = $(top_srcdir)/QCamera2/HAL


lib_LTLIBRARIES = libqcamera2.la

AM_CFLAGS = -Wall -Wextra -Werror

AM_CPPFLAGS = -DUSE_MEDIA_EXTENSIONS

libqcamera2_la_CPPFLAGS = \
        -I$(WORKSPACE)/frameworks/native/include/media/openmax \
        -I$(WORKSPACE)/display/display-hal/libgralloc \
        -I$(WORKSPACE)/display/display-hal/libqdutils \
        -I$(WORKSPACE)/display/display-hal/libqservice \
        -I$(WORKSPACE)/hardware/qcom/media/libstagefrighthw \
        -I$(WORKSPACE)/system/media/camera/include \
        -I$(WORKSPACE)/system/core/include \
        -I$(curr_path)/../../mm-image-codec/qexif \
        -I$(curr_path)/../../mm-image-codec/qomx_core \
        -I$(curr_path)/../stack/common \
        -I$(curr_path)/../util \
        -I$(curr_path)/wrapper

#QCamera2_la_CPPFLAGS += i$(TARGET_OUT_HEADERS)/qcom/display

libqcamera2_la_CPPFLAGS += -include stdint.h
#libqcamera2_la_CPPFLAGS += -include glib.h
#libqcamera2_la_CPPFLAGS += -include glibconfig.h
libqcamera2_la_CPPFLAGS += -include sys/ioctl.h
libqcamera2_la_CPPFLAGS += -include cutils/trace.h
libqcamera2_la_CPPFLAGS += -include sys/stat.h

libqcamera2_la_SOURCES = \
        QCamera2Factory.cpp \
        QCamera2Hal.cpp \
        QCamera2HWI.cpp \
        QCameraMem.cpp \
        ../util/QCameraQueue.cpp \
        ../util/QCameraCmdThread.cpp \
        QCameraStateMachine.cpp \
        QCameraChannel.cpp \
        QCameraStream.cpp \
        QCameraPostProc.cpp \
        QCamera2HWICallbacks.cpp \
        QCameraParameters.cpp \
        QCameraThermalAdapter.cpp \
        wrapper/QualcommCamera.cpp

libqcamera2_la_LIBADD = -ldl -lpthread -lglib-2.0 -lutils -lcutils -lhardware -lqdMetaData -lqservice -lbinder
libqcamera2_la_LIBADD += -lcamera_client -llog -lhardware -lutils -lcutils -ldl
libqcamera2_la_LIBADD += -lmmcamera_interface -lqdMetaData #-lmmjpeg_interface
#libqcamera2_la_LIBADD += -lts_face_beautify_hal -lts_detected_face_hal
libqcamera2_la_LIBADD += -lqdMetaData -lqservice -lbinder

libqcamera2_la_LIBADD += ../stack/mm-camera-interface/libmmcamera_interface.la
libqcamera2_la_LIBADD += ../stack/mm-jpeg-interface/libmmjpeg_interface.la
libqcamera2_la_LDFLAGS = -shared

