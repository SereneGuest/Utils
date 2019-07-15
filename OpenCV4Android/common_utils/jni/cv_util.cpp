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

void generateWatermark(unsigned char* yuvBuf, int width, int height,
        int stride, int scanline, int rotation, const char* logoPath)
{
    MY_LOGD("start load watermark file");
    // load watermark file and convert format.
    Mat srcMat = imread(logoPath, IMREAD_UNCHANGED);
    if (srcMat.empty()) {
        MY_LOGW("load watermark failed: %s", logoPath);
        return;
    }
    int scaleHeight = height / SCALE_SIZE;
    int scaleWidth = srcMat.cols / (float) srcMat.rows * scaleHeight;
    Mat logoMat(scaleHeight, scaleWidth, srcMat.type());
    MY_LOGD("start resize watermark");
    resize(srcMat, logoMat, logoMat.size(), 0, 0, INTER_LINEAR);
    int ySize = stride * scanline;
    // perform rotation
    int startPosition, startUvPosition;
    int xOffset, yOffset;
    MY_LOGD("start rotation watermark w:%d h:%d", logoMat.cols, logoMat.rows);
    switch (rotation) {
        case 90:
            xOffset = scaleWidth / 14;
            yOffset = scaleWidth / 6;
            rotate(logoMat, logoMat, ROTATE_90_CLOCKWISE);
            startPosition = yOffset * stride + xOffset;
            startUvPosition = ySize + yOffset * stride / 2 + xOffset;
            break;
        case 180:
            xOffset = scaleWidth / 6;
            yOffset = scaleWidth / 14;
            rotate(logoMat, logoMat, ROTATE_180);
            startPosition = width - logoMat.cols - xOffset + yOffset * stride;
            startUvPosition = ySize + (width - logoMat.cols - xOffset) +  yOffset * stride / 2;
            break;
        case 270:
            xOffset = scaleWidth / 14;
            yOffset = scaleWidth / 6;
            rotate(logoMat, logoMat, ROTATE_90_COUNTERCLOCKWISE);
            startPosition = (height - logoMat.rows - yOffset) * stride + (width - logoMat.cols - xOffset);
            startUvPosition = ySize + (height - logoMat.rows - yOffset) / 2 * stride + (width - logoMat.cols - xOffset);
            break;
        default:
            // use default, no rotate.
            xOffset = scaleWidth / 6;
            yOffset = scaleWidth / 14;
            startPosition = (height - logoMat.rows - yOffset) * stride + xOffset;
            startUvPosition = ySize + (height - logoMat.rows - yOffset) / 2 * stride + xOffset;
            break;
    }
    MY_LOGD("start copy watermark pixel");
    // copy data
    int yIndex, uvIndex, i, j;
    uchar value[3] = {0, 0, 0};
    Vec4b pixel;
    uchar* yuvPtr;
    for (i = 0; i < logoMat.rows; i++) {
        yIndex = startPosition + stride * i;
        uvIndex = startUvPosition + stride * i / 2;
        for (j = 0; j < logoMat.cols; j++) {
            pixel = logoMat.at<Vec4b>(i, j);
            if (pixel[3] == 0) {
                continue;
            }
            value[0] = (0.257 * pixel[2]) + (0.504 * pixel[1]) + (0.098 * pixel[0]) + 16 ;
            value[1] = -(0.148 * pixel[2]) - (0.291 * pixel[1]) + (0.439 * pixel[0]) + 128;
            value[2] = (0.439 * pixel[2]) - (0.368 * pixel[1]) - (0.071 * pixel[0]) + 128;
            yuvPtr = yuvBuf + yIndex + j;
            *(yuvPtr) = value[0] * pixel[3] / 255 +  (255 - pixel[3]) * (*yuvPtr) / 255;
            if (i % 2 == 0 && j % 2 == 0) {
                yuvPtr = yuvBuf + uvIndex + j;
                *(yuvPtr) = value[1] * pixel[3] / 255 + (255 - pixel[3]) * (*yuvPtr) / 255;
                *(yuvPtr + 1) = value[2] * pixel[3] / 255 + (255 - pixel[3]) * (*(yuvPtr + 1)) / 255;
            }
        }
    }
    MY_LOGD("end copy watermark pixel");
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
