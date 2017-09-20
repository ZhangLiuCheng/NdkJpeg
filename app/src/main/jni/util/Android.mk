LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES  += libjpeg
LOCAL_C_INCLUDES        += $(LOCAL_PATH)/../libjpeg-turbo


LOCAL_MODULE        = bitmap_util
LOCAL_SRC_FILES     := image_rar.c

LOCAL_LDLIBS        := -llog -ljnigraphics

include $(BUILD_SHARED_LIBRARY)