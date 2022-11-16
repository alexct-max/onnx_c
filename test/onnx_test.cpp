#include <iostream>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>
#include <chrono>
#include <string>
#include <core/session/onnxruntime_cxx_api.h>
#include "/home/alex/code/onnx_c/tools/processImg.cpp"
#include <cmath>
#include <cstdio>

using namespace std;
using namespace cv;
using namespace Ort;

int main(void)
{
    std::array<int64_t, 4> input_shape_{1, channel, height, width};
    // cout << names[10] << endl;
    // 图片路径、模型路径需要命令行给出
    string imgPath = "/home/alex/code/yolov7/inference/images/horses.jpg";
    string modelPath = "/home/alex/code/yolov7/model_path/yolov7-tiny.onnx";

    Mat imgSource_ = imread(imgPath);
    processImgData myData = process(imgSource_, width, height, 114., 32, true, false);
    Mat imgSource = myData.img;

    Env env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, "Detection");
    SessionOptions session_options;
    OrtSessionOptionsAppendExecutionProvider_CUDA(session_options, 0);
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
    Ort::AllocatorWithDefaultOptions allocator;
    //加载ONNX模型
    Ort::Session session(env, modelPath.c_str(), session_options);
    //获取输入输出的维度
    std::vector<int64_t> input_dims = session.GetInputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
    // std::cout << input_dims << endl;
    std::vector<int64_t> output_dims = session.GetOutputTypeInfo(0).GetTensorTypeAndShapeInfo().GetShape();
    std::vector<const char *> input_node_names = {"images"};
    std::vector<const char *> output_node_names = {"output"};
    input_dims[0] = output_dims[0] = 1; // batch size = 1

    std::vector<Ort::Value> input_tensors;
    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);

    //将图像存储到uchar数组中，BGR--->RGB
    std::array<float, width * height * channel> input_image_{};
    float *input = input_image_.data();
    for (int i = 0; i < imgSource.rows; i++)
    {
        for (int j = 0; j < imgSource.cols; j++)
        {
            for (int c = 0; c < 3; c++)
            {
                if (c == 0)
                    input[c * imgSource.rows * imgSource.cols + i * imgSource.cols + j] = imgSource.ptr<uchar>(i)[j * 3 + 2] / 255.;
                if (c == 1)
                    input[c * imgSource.rows * imgSource.cols + i * imgSource.cols + j] = imgSource.ptr<uchar>(i)[j * 3 + 1] / 255.;
                if (c == 2)
                    input[c * imgSource.rows * imgSource.cols + i * imgSource.cols + j] = imgSource.ptr<uchar>(i)[j * 3 + 0] / 255.;
            }
        }
    }

    input_tensors.push_back(Ort::Value::CreateTensor<float>(
        memory_info, input, input_image_.size(), input_shape_.data(), input_shape_.size()));
    //不知道输入维度时
    // input_tensors.push_back(Ort::Value::CreateTensor<uchar>(
    //        memory_info, input, input_image_.size(), input_dims.data(), input_dims.size()));

    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

    std::vector<Ort::Value> output_tensors;
    for (int i = 0; i < 100; i++)
        output_tensors = session.Run(Ort::RunOptions{nullptr},
                                     input_node_names.data(),   //输入节点名
                                     input_tensors.data(),      // input tensors
                                     input_tensors.size(),      // 1
                                     output_node_names.data(),  //输出节点名
                                     output_node_names.size()); // 4
    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();

    chrono::duration<double> delay_time = chrono::duration_cast<chrono::duration<double>>(t2 - t1); // milliseconds 毫秒

    cout << "前向传播平均耗时:" << delay_time.count() * 1000 / 100.0 << "ms" << endl;
    float *output_ = output_tensors[0].GetTensorMutableData<float>();

    cv::Mat frame(imgSource_.clone());

    float dw = myData.dw;
    float dh = myData.dh;
    float ratio = myData.ratio;
    //cout << dw << '\n' << dh << '\n' << ratio << endl;
    //cout << output_ << endl;
    if (output_ == 0)
    {
        cout << "未检出！" << endl;
        return 0;
    }
    for (int i = 0;; i += 7)
    {
        if (output_[i] == 0. and output_[i + 1] == 0. and output_[i + 2] == 0. and output_[i + 3] == 0. and output_[i + 4] == 0. and output_[i + 5] == 0. and output_[i + 6] == 0.)
        {
            break;
        }
        int xLeftBottom = static_cast<int>(output_[i + 1]);
        xLeftBottom -= dw;
        xLeftBottom /= ratio;
        int yLeftBottom = static_cast<int>(output_[i + 2]);
        yLeftBottom -= dh;
        yLeftBottom /= ratio;
        int xRightTop = static_cast<int>(output_[i + 3]);
        xRightTop -= dw;
        xRightTop /= ratio;
        int yRightTop = static_cast<int>(output_[i + 4]);
        yRightTop -= dh;
        yRightTop /= ratio;
        cv::Rect object((int)xLeftBottom, (int)yLeftBottom,
                        (int)(xRightTop - xLeftBottom),
                        (int)(yRightTop - yLeftBottom));
        int id = int(output_[i + 5]);
        string name = names[id];
        // cout << name << endl;
        cv::rectangle(frame, object, cv::Scalar(0, 0, 255), 2);
        cv::String label = name + cv::String(" confidence :") + to_string(output_[i + 6]);
        int baseLine = 0;
        cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.3, 1, &baseLine);
        cv::rectangle(frame, cv::Rect(cv::Point(xLeftBottom, yLeftBottom - labelSize.height), cv::Size(labelSize.width, labelSize.height + baseLine)),
                      cv::Scalar(255, 255, 0), cv::FILLED);
        cv::putText(frame, label, cv::Point(xLeftBottom, yLeftBottom),
                    cv::FONT_HERSHEY_SIMPLEX, 0.3, cv::Scalar(0, 0, 0));
    }
    cv::imshow("frame", frame);
    // cv::imwrite("test.jpg", frame);
    cv::waitKey(0);
    return 0;
}
