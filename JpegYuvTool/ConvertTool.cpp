#include "cmdline.h"
#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

using namespace cv;
using namespace std;

#define NO_FLIP -2
#define FLIP_HOR_VER -1
#define FLIP_VER 0
#define FLIP_HOR 1

bool isDir(const char* path)
{
    struct stat inputStat;
    stat(path, &inputStat);
    return S_ISDIR(inputStat.st_mode);
}
/* If path is directory and not end with '/' append '/' */
void fixDirPath(string &path)
{
    if (path.back() != '/') {
        path.append("/");
    }
}

/*
 * Can be file or directory
 */
void getFileList(const char* path, vector<string> &list)
{
    // is file , just return file name
    if (!isDir(path)) {
        cout << "path;" << path << endl;
        return list.push_back(path);
    }
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (path)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            string basePath(path);
            fixDirPath(basePath);
            //cout << basePath.append(ent->d_name) << " type:" << (int) ent->d_type << endl;
            if (ent->d_type == 8 /*file*/) {
                list.push_back(basePath.append(ent->d_name));
                cout << basePath << endl;
            }
        }
    closedir (dir);
    } else {
        cout << "can not open directory" << endl;
    }
}
/* Get and check parameters from cmd */
void initCmdParameters(cmdline::parser &argsParser, int argc, char** argv)
{
    // 1. 参数名称 2. 缩写 3. 参数描述 4. 是否必须输入 5. 不是必须情况下 默认值 6. 输入值限制
    argsParser.add<string>("input", 'i', "input path or file name", true, "");
    argsParser.add<string>("output", 'o', "output path", true, "");
    argsParser.add<int>("width", 'w', "image width, no need to set for read yuv file", false, 0);
    argsParser.add<int>("height", 'h', "image height, no need to set for read yuv file", false, 0);
    argsParser.add<int>("stride", 'W', "for yuv file, image stride, stride >= width", false, 0);
    argsParser.add<int>("scanline", 'H', "for yuv file image scanline, scanline >= height", false, 0);
    string formatDes = "convert format, can be range 0 - 7 : \n" \
                        "\t \t \t 0 --> NV21 to JPEG \n" \
                        "\t \t \t 1 --> NV12 to JPEG \n" \
                        "\t \t \t 2 --> YV12 to JPEG \n" \
                        "\t \t \t 3 --> I420 to JPEG \n" \
                        "\t \t \t 4 --> JPEG to NV21 \n" \
                        "\t \t \t 5 --> JPEG to NV12 \n" \
                        "\t \t \t 6 --> JPEG to YV12 \n" \
                        "\t \t \t 7 --> JPEG to I420 \n";
    argsParser.add<int>("format", 'F',  formatDes, true, 0, cmdline::range(0, 7));
    argsParser.add<int>("rotation", 'r', "rotation image clock wise",
            false, 0, cmdline::oneof<int>(0, 90, 180, 270));
    argsParser.add<int>("flip", 'f', "flip image -2:no-flip, -1:flip-hor-ver 0:flip-ver 1:flip-hor",
            false, -2, cmdline::oneof<int>(-2, -1, 0, 1));
    argsParser.add<int>("quality", 'q', "jpeg compress quality default:90", false, 90, cmdline::range(0, 100));
    argsParser.parse_check(argc, argv);
    cout << "input:" << argsParser.get<string>("input") << endl;
    cout << "output:" << argsParser.get<string>("output") << endl;
    cout << "width:" << argsParser.get<int>("width") << endl;
    cout << "height:" << argsParser.get<int>("height") << endl;
    cout << "stride:" << argsParser.get<int>("stride") << endl;
    cout << "scanline:" << argsParser.get<int>("scanline") << endl;
    cout << "format:" << argsParser.get<int>("format") << endl;
    cout << "rotation:" << argsParser.get<int>("rotation") << endl;
    cout << "flip:" << argsParser.get<int>("flip") << endl;
    cout << "quality:" << argsParser.get<int>("quality") << endl;
}

/* Check whether image size is valid, only need check when file is yuv
   because jpeg file size info can be get from file */
bool checkParameters(cmdline::parser &cmdParser) {
    string outPath = cmdParser.get<string>("output");
    if (!isDir(outPath.c_str())) {
        cout << "error: out path is not a dir" << endl;
        return false;
    }
    // not yuv file, no need check image size info
    if (cmdParser.get<int>("format") >= 4) {
        return true;
    }
    // check image size info for yuv file
    int width = cmdParser.get<int>("width");
    int height = cmdParser.get<int>("height");
    int stride = cmdParser.get<int>("stride");
    int scanline = cmdParser.get<int>("scanline");
    if (stride <= 0 || scanline <= 0) {
        cout << "error: stride(-W) and scanline(-H) must > 0" << endl;
        return false;
    }
    if (stride < width || scanline < height) {
        cout << "error: stride&scanline must bigger than width&height" << endl;
        return false;
    }
    return true;
}

/* Read data from file, return char array */
char* getYuvBuffer(string file)
{
    long size;
    cout << "file path:" << file << endl;
    ifstream inputFile(file, ios::in | ios::binary | ios::ate);
    size = inputFile.tellg();
    cout << "size:" << size << endl;
    inputFile.seekg(0, ios::beg);
    char* yuvBuffer = new char[size];
    inputFile.read(yuvBuffer, size);
    inputFile.close();
    return yuvBuffer;
}

/* Perform rotation and flip var parameters */
void performRotateAndFlip(Mat &src, int flipType, int rotation)
{
    if (flipType != NO_FLIP) {
        flip(src, src, flipType);
    }
    if (rotation != 0) {
        RotateFlags flag = RotateFlags(rotation / 90 -1);
        cv::rotate(src, src, flag);
    }
}

/* Get out file name, outPath + oriFileName + suffix */
string getOutFileName(string path, string oriFile, const char* suffix)
{
    fixDirPath(path);
    size_t found = oriFile.find_last_of('/');
    path.append(oriFile.substr(found + 1)).append(suffix);
    return path;
}

/* Convert yuv file to jpeg file */
void yuv2Jpeg(cmdline::parser &cmdParser, string file, ColorConversionCodes code)
{
    // read file data
    char* data = getYuvBuffer(file);
    // get relative cmd parameters
    int width = cmdParser.get<int>("width");
    int height = cmdParser.get<int>("height");
    int stride = cmdParser.get<int>("stride");
    int scanline = cmdParser.get<int>("scanline");
    int flipType = cmdParser.get<int>("flip");
    int rotation = cmdParser.get<int>("rotation");
    int quality = cmdParser.get<int>("quality");
    string outPath = cmdParser.get<string>("output");
    // create Mat
    Mat yuvMat(scanline * 3 / 2, stride, CV_8UC1, data, Mat::AUTO_STEP);
    Mat bgrMat(scanline, stride, CV_8UC3);
    // convert to bgr
    cvtColor(yuvMat, bgrMat, code);
    // remove padding for yuv file only when (stride&scanline > width&height) && (width&height > 0)
    if (width == 0 || height == 0) {
        width = stride;
        height = scanline;
    }
    Mat cropMat = bgrMat(cv::Rect(0, 0, width, height));
    // flip and rotate
    performRotateAndFlip(cropMat, flipType, rotation);
    // jpeg quality
    // write to jpeg file
    vector<int> params;
    params.push_back(IMWRITE_JPEG_QUALITY);
    params.push_back(quality);
    // write to file
    string outName = getOutFileName(outPath, file, ".jpeg");
    cout << "outName:" << outName << endl;
    cv::imwrite(outName, cropMat, params);
    delete [] data;
}

void jpeg2Yuv(cmdline::parser &cmdParser, string file, ColorConversionCodes code)
{
    cout << "jpeg2Yuv:" << code << endl;
}

void jpeg2YuvExtra(cmdline::parser &cmdParser, string file, ColorConversionCodes code)
{
    cout << "jpeg2YuvExtra:" << code << endl;
}

void checkAndProcess(cmdline::parser &cmdParser, string file)
{
    int format = cmdParser.get<int>("format");
    switch (format)
    {
        case 0:
            /* NV21 to JPEG */
            yuv2Jpeg(cmdParser, file, COLOR_YUV2BGR_NV21);
            break;
        case 1:
            /* NV12 to JPEG */
            yuv2Jpeg(cmdParser, file, COLOR_YUV2BGR_NV12);
            break;
        case 2:
            /* YV12 to JPEG */
            yuv2Jpeg(cmdParser, file, COLOR_YUV2BGR_YV12);
            break;
        case 3:
            /* I420 to JPEG */
            yuv2Jpeg(cmdParser, file, COLOR_YUV2BGR_I420);
            break;
        case 4:
            /* JPEG to NV21, because opencv not support gbr to NV21,
               So we need manual convert YV12 to NV21 for */
            jpeg2YuvExtra(cmdParser, file, COLOR_BGR2YUV_YV12);
            break;
        case 5:
            /* JPEG to NV12, because opencv not support gbr to NV12,
               So we need manual convert Int20 to NV12 for */
            jpeg2YuvExtra(cmdParser, file, COLOR_BGRA2YUV_I420);
            break;
        case 6:
            /* JPEG to YV12 */
            jpeg2Yuv(cmdParser, file, COLOR_BGR2YUV_YV12);
            break;
        case 7:
            /* JPEG to I420 */
            jpeg2Yuv(cmdParser, file, COLOR_BGR2YUV_I420);
            break;
        default:
            // should never happen
            cout << "error:format not support" << endl;
            break;
    }

}

int main(int argc, char** argv)
{
    // init cmd parameters
    cmdline::parser cmdParser;
    initCmdParameters(cmdParser, argc, argv);
    // check parameters
    if (!checkParameters(cmdParser)) {
        return -1;
    }
    // get input file info
    vector<string> fileLists;
    getFileList(cmdParser.get<string>("input").c_str(), fileLists);
    // check format and start convert
    checkAndProcess(cmdParser, fileLists[0]);
    return 0;
}
