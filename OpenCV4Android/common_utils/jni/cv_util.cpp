#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <stdlib.h>
#include "common.h"
#include "cv_util.h"

using namespace cv;
using namespace std;

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
