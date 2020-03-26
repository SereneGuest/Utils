#ifndef __WZ_CV_UTIL_H__
#define __WZ_CV_UTIL_H__

#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <android/log.h>

#define CV_TAG "CVUtil"
#define MY_LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, CV_TAG, __VA_ARGS__)
#define MY_LOGI(...) __android_log_print(ANDROID_LOG_INFO, CV_TAG, __VA_ARGS__)
#define MY_LOGW(...) __android_log_print(ANDROID_LOG_WARN, CV_TAG, __VA_ARGS__)
#define MY_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, CV_TAG, __VA_ARGS__)
#define MY_LOGF(...) __android_log_print(ANDROID_LOG_FATAL, CV_TAG, __VA_ARGS__)

#define MAX_FILE_LENGTH 128
#define MAX_TIME_LENGTH 64
#define SCALE_SIZE 8

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
 * Must be NV21 YUV format, no align alignSize = 0
 */
void dumpJpeg(int width, int height, int alignSize,
        unsigned char* buffer, const char* path);

/**
 * Draw watermark in NV21 buffer, watermark load from file.
 * @param yuv buffer address.
 * @param width yuv image width.
 * @param height yuv image heght.
 * @param stride yuv buffer stride.
 * @param scanline yuv buffer height.
 * @param rotation watermark rotate value, clockwise (0, 90, 180, 270).
 * @param logoPath watermark file path, can be jpeg or png.
 */
void generateWatermark(unsigned char* yuvBuf, int width, int height,
        int stride, int scanline, int rotation, const char* logoPath);

#ifdef __cplusplus
} // extern C
#endif

#endif //__WZ_CV_UTIL_H__
