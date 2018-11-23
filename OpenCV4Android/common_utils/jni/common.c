#include "common.h"

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