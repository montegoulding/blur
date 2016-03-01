LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

TARGET_PLATFORM=android-8

LOCAL_MODULE := $(EXTERNAL_NAME)

LOCAL_SRC_FILES := blur.cpp external/external.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/external \

LOCAL_LDFLAGS += -Wl,-u,getXtable

include $(BUILD_SHARED_LIBRARY)