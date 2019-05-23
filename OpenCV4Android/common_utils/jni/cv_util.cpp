#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <stdlib.h>
#include "cv_util.h"

using namespace cv;
using namespace std;

int dumpFile(void* buffer, long size, const char* path) {
    FILE* fp = fopen(path, "w+");
    if (fp != NULL) {
        fwrite(buffer, 1, size, fp);
        fclose(fp);
    } else {
        MY_LOGE("file open error");
        return -1;
    }
    return 0;
}

int readFile(void* buffer, long size, const char* path) {
    FILE* fp = fopen(path, "r");
    if (fp != NULL) {
        fread(buffer, 1, size, fp);
        fclose(fp);
    } else {
        MY_LOGE("file open error");
        return -1;
    }
    return 0;
}

void getPathYuv(int width, int height, int stride, int scanline,
        const char* path, const char* tag, char* pathBuf) {
    char time[MAX_TIME_LENGTH];
    getTimeStr(time);
    sprintf(pathBuf, "%simg-%s_%d_%d_%d_%d_%s.yuv",
            path, time, width, height, stride, scanline, tag);
}

void getPathJpeg(const char* path, const char* tag, char* pathBuf) {
    char time[MAX_TIME_LENGTH];
    getTimeStr(time);
    sprintf(pathBuf, "%simg_%s_%s.jpeg", path, time, tag);
}

void getTimeStr(char* resultTime) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    struct tm *tmpTime;
    tmpTime = localtime(&tv.tv_sec);
    sprintf(resultTime, "%d-%d-%d-%d-%d-%d-%d", tmpTime->tm_year + 1900,
            tmpTime->tm_mon + 1, tmpTime->tm_mday, tmpTime->tm_hour,
            tmpTime->tm_min, tmpTime->tm_sec, (int) (tv.tv_usec / 1000));
}

int align(int align, int target) {
    return (target + align - 1) & (~(align - 1));
}

void dumpJpeg(int width, int height, int alignSize,
        unsigned char* buffer, const char* path) {
    MY_LOGD("info %d %d %d %p %s", width, height, alignSize, buffer, path);
    int realWidth = align(alignSize, width);
    int realHeight = align(alignSize, height);
    Mat yuvMat(realHeight * 3 / 2, realWidth, CV_8UC1, buffer, 0/*AUTO_STEP*/);
    Mat bgrMat(realHeight, realWidth, CV_8UC3);
    //convert color
    cvtColor(yuvMat, bgrMat, COLOR_YUV2BGR_NV21);
    imwrite(path, bgrMat);
}

int main(int argc, char** argv)
{
    /*
    char timeStr[MAX_FILE_LENGTH];
    getTimeStr(timeStr);
    char yuvPath[MAX_FILE_LENGTH];
    getPathYuv(2592, 1944, 2592, 1952, "/data/misc/camera/", "tset", yuvPath);
    char jpegPath[MAX_FILE_LENGTH];
    getPathJpeg("/data/misc/camera/", "demo", jpegPath);
    MY_LOGD("%s", timeStr);
    MY_LOGD("%s", yuvPath);
    MY_LOGD("%s", jpegPath);
    */
    return 0;
}
