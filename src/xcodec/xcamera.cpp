#include "xcamera.h"
#include "xtools.h"
#include "xvideo_view.h"
#include<iostream>

using namespace std;
extern "C" {
#include<libavformat/avformat.h>
#include<libavutil/opt.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

//预指令导入库
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib, "swresample.lib")


bool XCamera::Init(const char* name, bool is_video)
{
	if (inputFormatContext_)
	{
		avformat_close_input(&inputFormatContext_);
	}
	device_name_ = name;

	// Open video device
	avdevice_register_all();
	AVInputFormat* inputFormat = av_find_input_format("dshow");
	if (!inputFormat) {
		cerr << "Could not find input format" << endl;
		return false;
	}

	AVDictionary* options = nullptr;
	//av_dict_set(&options, "rtbufsize", "30412800", 0); // Increase buffer size
	//
	auto re = avformat_open_input(&inputFormatContext_, name, inputFormat, &options);
	if (re != 0)
	{
		cerr << "Could not open input device" << endl;
		return false;
	}

	av_dict_free(&options);

	re = avformat_find_stream_info(inputFormatContext_, NULL);
	if (re < 0)
	{
		cerr << "Could not find stream info" << endl;
		return false;
	}
	av_dump_format(inputFormatContext_, 0, name, 0);


	if (!Init_video())
	{
		cerr << "init video module error" << endl;
		return false;
	}

	return true;

}



bool XCamera::Init_video()
{
	for (int i = 0; i < inputFormatContext_->nb_streams; i++)
	{
		if (inputFormatContext_->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStreamIndex_ = i;
		}
	}
	if (videoStreamIndex_ == -1)
	{
		cout << "Could not find video stream" << endl;
		return false;
	}

	int format_index_ = inputFormatContext_->streams[videoStreamIndex_]->codecpar->format;
	cout << "code_type: " << format_index_ << endl;

	width_ = inputFormatContext_->streams[videoStreamIndex_]->codecpar->width;
	height_ = inputFormatContext_->streams[videoStreamIndex_]->codecpar->height;


	//初始化转换接口
		//RGB->yuv打开
	yuyv2yuv_ = sws_getCachedContext(
		yuyv2yuv_,				//转换上下文，NULL新常见，非NULL判断与现在参数是否一致，一致直接返回，不一致再次创建
								//一致直接返回，不一致先清理当前然后再创建
		width_, height_,			//输入宽高
		(AVPixelFormat)format_index_,		//输入像素格式BGRA
		width_, height_,   //输出宽高
		AV_PIX_FMT_YUV420P,         //输出像素格式
		SWS_BILINEAR,			//选择支持变换的算法，双线性插值
		NULL, NULL, NULL                 //过滤器参数
	);

	if (!yuyv2yuv_)
	{
		cerr << "create sws error" << endl;
		return false;
	}

	if (frame_)
	{
		av_frame_free(&frame_);
		frame_ = nullptr;
	}
	//初始化播放AVFrame
	frame_ = av_frame_alloc();
	frame_->width = width_;
	frame_->height = height_;
	frame_->format = AV_PIX_FMT_YUV420P;
	//生成图像空间，默认32位字节对齐
	auto re = av_frame_get_buffer(frame_, 1);

	frame_->linesize[0] = width_;
	frame_->linesize[1] = width_ / 2;
	frame_->linesize[2] = width_ / 2;


	return true;
}


AVPacket* XCamera::recive_frame()
{
	AVPacket* pkt = av_packet_alloc();
	unique_lock<mutex> lock(mux_);
	auto re = av_read_frame(inputFormatContext_, pkt);
	if (re >= 0)
	{
		return pkt;
	}
	return nullptr;
}

AVFrame* XCamera::get_frame(AVPacket* pkt)
{
	unique_lock<mutex> lock(mux_);
	uint8_t* data[AV_NUM_DATA_POINTERS] = { 0 };
	int line[AV_NUM_DATA_POINTERS] = { 0 };


	data[0] = (uint8_t*)pkt->data;
	line[0] = pkt->size / height_;
	int re = sws_scale(yuyv2yuv_,
		data,        //输入数据
		line,
		0,
		height_,
		frame_->data,
		frame_->linesize
	);
	if (re <= 0)
	{
		cerr << "trans error" << endl;
		return nullptr;
	}
	cout << "F" << flush;
	av_packet_unref(pkt);

	//返回复制
	auto frame = av_frame_alloc();
	frame->height = frame_->height;
	frame->width = frame_->width;
	frame->format = frame_->format;
	re = av_frame_ref(frame, frame_);
	if (re != 0)
	{
		av_frame_free(&frame);
		LOGERROR("XDecodeTask::GetFrame() ERROR");
		return nullptr;
	}
	return frame;
}


void XCamera::Close()
{
	if (inputFormatContext_)
	{
		avformat_close_input(&inputFormatContext_);
		avformat_free_context(inputFormatContext_);
		inputFormatContext_ = nullptr;
	}

	if (inputformat_)
	{
		// AVInputFormat 资源不需要手动释放，因为它通常是静态分配的。
		inputformat_ = nullptr;
	}

	device_name_ = nullptr;

	videoStreamIndex_ = -1;
	audioStreamIndex_ = -1;
	format_index_ = -1;

	if (yuyv2yuv_)
	{
		sws_freeContext(yuyv2yuv_);
		yuyv2yuv_ = nullptr;
	}

	width_ = 0;
	height_ = 0;

	if (frame_)
	{
		av_frame_free(&frame_);
		frame_ = nullptr;
	}
}