#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <chrono>
#include <string>
#include <vector>
#include <core/session/onnxruntime_cxx_api.h>
#include "/home/alex/code/onnx_c/tools/declare.cpp"
#include <jsoncpp/json/json.h>
#include <libgen.h>

using namespace std;
using namespace cv;

processImgData process(Mat img, int new_width, int new_height, double color, int stride, bool scaleup, bool auTo)
{
    processImgData imgData;
    Mat midImg, finalImg;
    int win_h = img.rows;
    int win_w = img.cols; // cols 640w0  rows 536h1
    float r = min((new_width * 1.0f) / (win_w * 1.0f), (new_height * 1.0f) / (win_h * 1.0f));
    if (not scaleup)
    {
        r = min(r, 1.0f);
    }
    int new_unpad0 = int(round(win_w * r));
    int new_unpad1 = int(round(win_h * r));
    int dw = new_height - new_unpad0;
    int dh = new_width - new_unpad1;
    if (auTo)
    {
        dw = dw % stride;
        dh = dh % stride;
    }
    dw /= 2;
    dh /= 2;

    if (win_w != new_unpad1 or win_h != new_unpad0)
    {
        resize(img, midImg, cv::Size(new_unpad0, new_unpad1), 0.0, 0.0, cv::INTER_LINEAR);
    }
    else
    {
        midImg = img;
    }
    int top = int(round(dh - 0.1));
    int bottom = int(round(dh + 0.1));
    int left = int(round(dw - 0.1));
    int right = int(round(dw + 0.1));
    copyMakeBorder(midImg, finalImg, top, bottom, left, right, cv::BORDER_CONSTANT, cv::Scalar(color, color, color));
    imgData.img = finalImg;
    imgData.dw = dw;
    imgData.dh = dh;
    imgData.ratio = r;
    return imgData;
}

void postProcessImg(cv::Mat img, std::string imgName, std::string outputPath, float *outputTentor, float ratio, float dw, float dh, bool showImg, bool saveImg, bool saveJson)
{
}

string get_current_directory()
{
    char buff[maxFileValue];
    GetCurrentDir(buff, maxFileValue);
    string current_working_directory(buff);
    return current_working_directory;
}

imgNameAndPath getImgPathAndName(std::string path)
{
    imgNameAndPath myData;
    string outname_base = path;
    char *p = (char *)outname_base.c_str();
    string outname = basename(p); //最后斜杠之后的文件名
    string outdir = dirname(p);
    cout << outdir << " -- " << outname << endl;
    myData.imgPath = outdir;
    myData.imgName[0] = outname;
    return myData;
}