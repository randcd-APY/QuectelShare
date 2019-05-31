SRS_PROCESSING_ROOT := $(call my-dir)

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
ifneq ($(filter P% p%,$(TARGET_PLATFORM_VERSION)),)   # ANDROID_P
LOCAL_MODULE := lib_audio_srs_headers
LOCAL_EXPORT_C_INCLUDE_DIRS := TruMedia
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_COPY_HEADERS)
else
LOCAL_COPY_HEADERS_TO := mm-audio/audio-effects
LOCAL_COPY_HEADERS := TruMedia/srs_processing.h
LOCAL_COPY_HEADERS += TruMedia/postpro_patch.h
include $(BUILD_COPY_HEADERS)
endif

