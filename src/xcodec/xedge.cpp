#include "xedge.h"

#include<opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "avutil.lib")

#pragma comment(lib,"opencv_core3410d.lib")
#pragma comment(lib,"opencv_imgproc3410d.lib")
#pragma comment(lib,"opencv_imgcodecs3410d.lib")
#pragma comment(lib,"opencv_highgui3410d.lib")
#pragma comment(lib,"opencv_ml3410d.lib")
#pragma comment(lib,"opencv_video3410d.lib")

#include <algorithm> // for std::min

using namespace std;
using namespace cv;

bool XEdge::Init()
{
    return true;
}

PData* XEdge::process(PData* f)
{
    if (stream_index_ == 0) {
        auto a = NowMs();
        AVFrame* frame = f->data_;
        if (!frame) {
            cout << "Input frame is null.";
            return f;
        }

        cv::Mat src = AVFrameToCVMat(frame);
        if (src.empty()) {
            cout << "Conversion from AVFrame to cv::Mat failed.";
            return f;
        }

        // 创建颜色空间转换前的副本
        cv::Mat clonedMat = src.clone();

        cv::Mat grayImage;
        cv::cvtColor(clonedMat, grayImage, cv::COLOR_BGR2GRAY);

        // 使用 Cany 算子进行边缘检测
        Mat dstImage, edge;

        blur(grayImage, grayImage, Size(3, 3));
        Canny(grayImage, edge, 150, 100, 3);

        dstImage.create(clonedMat.size(), clonedMat.type());
        dstImage = Scalar::all(0);
        clonedMat.copyTo(dstImage, edge);

        // 将处理后的图像转换回 AVFrame
        AVFrame* out_frame = CVMatToAVFrame(dstImage);
        if (!out_frame) {
            cout << "Conversion from cv::Mat to AVFrame failed.";
            return f;
        }

        // 释放输入帧的内存
        av_frame_free(&frame);

        // 设置输出帧数据
        f->data_ = out_frame;

        cout << "==================" << NowMs() - a << endl;
    }


    return f;
}
