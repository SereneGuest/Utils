#ifndef __WZ_CV_UTIL_H__
#define __WZ_CV_UTIL_H__

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <android/log.h>

#define TAG "tag_wenzhe"
#define MY_LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define MY_LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define MY_LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define MY_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define MY_LOGF(...) __android_log_print(ANDROID_LOG_FATAL, TAG, __VA_ARGS__)

#define MAX_FILE_LENGTH 128
#define MAX_TIME_LENGTH 64

#ifdef __cplusplus
extern "C" {
#endif

int dumpFile(void* buffer, long size, const char* path);

int readFile(void* buffer, long size, const char* path);

void getPathYuv(int width, int height, int stride, int scanline,
        const char* path, const char* tag, char* pathBuf);

void getPathJpeg(const char* path, const char* tag, char* pathBuf);

void getTimeStr(char* resultTime);

int align(int align, int target);

/*
 * Must be NV21 YUV format
 */
void dumpJpeg(int width, int height, int alignSize,
        unsigned char* buffer, const char* path);

#ifdef __cplusplus
} // extern C
#endif

#endif //__WZ_CV_UTIL_H__
