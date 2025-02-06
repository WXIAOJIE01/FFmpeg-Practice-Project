#include "xfilter.h"
#include "xedge.h"
#include "xbilateral_filter.h"
#include<opencv2/opencv.hpp>

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

using namespace std;

XFilter* XFilter::create(FilterType type)
{
	switch (type)
	{
	case FilterType::raw:
		return nullptr;
		break;
	case FilterType::bilateralFilter:
		return new XBilateralFilter;
		break;
	case FilterType::edge:
		return new XEdge;
		break;
	default:
		break;
	}
}

cv::Mat XFilter::AVFrameToCVMat(AVFrame* frame)
{
	//�õ�AVFrame��Ϣ
	int srcW = frame->width;
	int srcH = frame->height;
	SwsContext* swsCtx = sws_getContext(srcW, srcH, (AVPixelFormat)frame->format, srcW, srcH, (AVPixelFormat)AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

	//����Mat����
	cv::Mat mat;
	mat.create(cv::Size(srcW, srcH), CV_8UC3);

	//��ʽת����ֱ�����Mat������data
	AVFrame* bgr24Frame = av_frame_alloc();
	av_image_fill_arrays(bgr24Frame->data, bgr24Frame->linesize, (uint8_t*)mat.data, (AVPixelFormat)AV_PIX_FMT_BGR24, srcW, srcH, 1);
	sws_scale(swsCtx, (const uint8_t* const*)frame->data, frame->linesize, 0, srcH, bgr24Frame->data, bgr24Frame->linesize);

	//�ͷ�
	av_frame_free(&bgr24Frame);
	sws_freeContext(swsCtx);

	return mat;

}

AVFrame* XFilter::CVMatToAVFrame(cv::Mat& inMat)
{
	AVPixelFormat dstFormat = AV_PIX_FMT_YUV420P;
	int width = inMat.cols;
	int height = inMat.rows;

	//����AVFrame������ ע���������ͷŸ�frame
	AVFrame* frame = av_frame_alloc();
	frame->width = width;
	frame->height = height;
	frame->format = dstFormat;

	//��ʼ��AVFrame�ڲ��ռ�
	int ret = av_frame_get_buffer(frame, 32);
	if (ret < 0)
	{
		cout << "Could not allocate the video frame data";
		return nullptr;
	}
	ret = av_frame_make_writable(frame);
	if (ret < 0)
	{
		cout << "Av frame make writable failed.";
		return nullptr;
	}

	//ת����ɫ�ռ�ΪYUV420
	cv::Mat outMat;
	cv::cvtColor(inMat, outMat, cv::COLOR_BGR2YUV_I420);

	//��YUV420��ʽ���������ݵ�ַ
	int frame_size = width * height;
	unsigned char* data = outMat.data;
	memcpy(frame->data[0], data, frame_size);
	memcpy(frame->data[1], data + frame_size, frame_size / 4);
	memcpy(frame->data[2], data + frame_size * 5 / 4, frame_size / 4);

	return frame;
}
