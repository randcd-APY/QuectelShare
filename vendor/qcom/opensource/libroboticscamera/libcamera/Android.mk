ifneq (,$(filter $(TARGET_ARCH), arm arm64))

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
        src/camera_memory.cpp \
        src/camera_parameters.cpp \
	src/qcamera2.cpp \
	src/camera_util_stereo.cpp

LOCAL_CFLAGS := -Wall -Wextra -Werror -std=c++11 -fexceptions

LOCAL_C_INCLUDES := \
        $(LOCAL_PATH)/inc \
        $(LOCAL_PATH)/src \
        $(LOCAL_PATH)/../QCamera2/HAL \
        $(LOCAL_PATH)/../QCamera2/util \
        $(LOCAL_PATH)/../QCamera2/stack/common \
        $(LOCAL_PATH)/../mm-image-codec/qomx_core \
        $(LOCAL_PATH)/../mm-image-codec/qexif


LOCAL_SHARED_LIBRARIES := libdl libcamera_metadata libcamera_client
LOCAL_COPY_HEADERS := inc/camera.h inc/camera_parameters.h

LOCAL_COPY_HEADERS_TO := libcamera/


LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE := libcamera
LOCAL_CLANG := false
LOCAL_MODULE_TAGS := optional


include $(BUILD_SHARED_LIBRARY)

$(local-intermediates-dir)/src/camera_parameters.o: PRIVATE_CFLAGS += -std=c++11

## build camera-test ##

include $(CLEAR_VARS)

LOCAL_MODULE := camera-test

LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../QCamera2/HAL

LOCAL_SRC_FILES := test/camera_test.cpp

LOCAL_SHARED_LIBRARIES := libcamera libcamera_metadata libcamera_client
#LOCAL_SHARED_LIBRARIES += turbojpeg

include $(BUILD_EXECUTABLE)


## build camera-stereo-raw-test ##

include $(CLEAR_VARS)

LOCAL_CFLAGS := -Wall -Wextra -Werror -std=c++11 -fexceptions

LOCAL_MODULE := camera-stereo-test

LOCAL_C_INCLUDES := $(LOCAL_PATH)/inc
LOCAL_C_INCLUDES += $(LOCAL_PATH)/src
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../QCamera2/HAL

LOCAL_SRC_FILES := test/camera_stereo_test.cpp

LOCAL_SHARED_LIBRARIES := libcamera libcamera_metadata libcamera_client


include $(BUILD_EXECUTABLE)

endif
