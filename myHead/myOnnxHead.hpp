#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

// 定义系统获取根目录方式及根目录最大字节
extern const int maxFileValue;
#if defined(_MSC_VER)
    #include <direct.h>
    #define GetCurrentDir _getcwd
#elif defined(__unix__)
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif
// 定义图像预处理后输出数据结构
struct processImgData
{
    cv::Mat img;
    float ratio;
    float dw;
    float dh;
};
// 定义图片输入尺寸
extern const int width;
extern const int height;
extern const int channel;
// 定义标签
extern const std::string names[];
// 定义获取图片名称、目录结构
struct imgNameAndPath
{
    std::string imgPath;
    std::vector<std::string> imgName; 
}; 