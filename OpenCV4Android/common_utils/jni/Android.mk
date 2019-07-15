LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#OPENCV_INSTALL_MODULES:=off
# OpenCV-android-sdk path
OPENCV_ANDROID_SDK := /mnt/e/linux/OpenCV-android-sdk
OPENCV_LIB_TYPE := STATIC
#OPENCV_LIB_TYPE := SHARED

ifdef OPENCV_ANDROID_SDK
  ifneq ("","$(wildcard $(OPENCV_ANDROID_SDK)/OpenCV.mk)")
    include ${OPENCV_ANDROID_SDK}/OpenCV.mk
  else
    include ${OPENCV_ANDROID_SDK}/sdk/native/jni/OpenCV.mk
  endif
else
  include ../../sdk/native/jni/OpenCV.mk
endif

LOCAL_SRC_FILES  := cv_util.cpp

LOCAL_C_INCLUDES += $(LOCAL_PATH)
LOCAL_LDLIBS     += -llog -ldl

# used in build executable.
#LOCAL_CFLAGS += -pie -fPIE
#LOCAL_LDFLAGS += -pie -fPIE

LOCAL_MODULE     := cv_util
#include $(BUILD_EXECUTABLE)
include $(BUILD_SHARED_LIBRARY)