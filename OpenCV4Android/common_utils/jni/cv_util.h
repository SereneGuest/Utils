#ifndef __WZ_CV_UTIL_H__
#define __WZ_CV_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Must be NV21 YUV format
 */
void dumpJpeg(int width, int height, int alignSize,
        unsigned char* buffer, const char* path);

#ifdef __cplusplus
} // extern C
#endif

#endif //__WZ_CV_UTIL_H__
