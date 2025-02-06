#include "xbilateral_filter.h"
#include<opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

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

bool XBilateralFilter::Init()
{
    return true;
}

PData* XBilateralFilter::process(PData* f)
{
    if (stream_index_ == 0) {
        auto ti = NowMs();
        AVFrame* frame = f->data_;
        Mat src = AVFrameToCVMat(frame);
        Mat dst;
        int d = 5;
        cv::bilateralFilter(src, dst, d, d * 2, d / 2);
        AVFrame* out_frame = CVMatToAVFrame(dst);
        av_frame_free(&f->data_);
        src.release();
        dst.release();
        f->data_ = out_frame;
        cout << "(" << ti - NowMs() << ")" << endl;
    }
    else if (stream_index_ == 1)
    {

    }
    return f;
}
