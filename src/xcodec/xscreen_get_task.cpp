#include "xscreen_get_task.h"
#include <d3d9.h>
#include<iostream>
using namespace std;

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")

#pragma comment(lib, "d3d9.lib")

void XScreen_get_task::Open()
{
	if (!screen_.Init())
	{
		cerr << "directX init error" << endl;
		return;
	}
	w_ = screen_.get_width();
	h_ = screen_.get_height();

	size_ = w_ * h_ * 4;
	buf_ = new unsigned char[size_];


	//RGB->yuv打开
	rgb2yuv_ = sws_getCachedContext(
		rgb2yuv_,				//转换上下文，NULL新常见，非NULL判断与现在参数是否一致，一致直接返回，不一致再次创建
								//一致直接返回，不一致先清理当前然后再创建
		w_, h_,			//输入宽高
		(AVPixelFormat)28,		//输入像素格式BGRA
		w_, h_,   //输出宽高
		AV_PIX_FMT_YUV420P,         //输出像素格式
		SWS_BILINEAR,			//选择支持变换的算法，双线性插值
		NULL, NULL, NULL                 //过滤器参数
	);
	if (!rgb2yuv_)
	{
		cerr << "sws_getCachedContext failed" << endl;
		return;
	}

}

void XScreen_get_task::Main()
{
	auto device = screen_.get_device();
	auto sur = screen_.get_sur();
	if (!device || !sur) return;
	while (!is_exit_)
	{
		//4 抓屏
		device->GetFrontBufferData(0, sur);

		//5 取出数据
		D3DLOCKED_RECT rect;
		ZeroMemory(&rect, sizeof(rect));
		if ((sur->LockRect(&rect, 0, 0)) != S_OK)
		{
			cerr << "get sur error" << endl;
		}
		memcpy(buf_, rect.pBits, size_);

		//释放锁
		sur->UnlockRect();

		long long vpts = NowMs();
		cout << ".";
		//开始处理
		uint8_t* data[AV_NUM_DATA_POINTERS] = { 0 };
		int line[AV_NUM_DATA_POINTERS] = { 0 };

		data[0] = (uint8_t*)buf_;
		line[0] = w_ * 4;

		//创建frame
		auto frame = av_frame_alloc();
		frame->height = h_;
		frame->width = w_;
		frame->format = AV_PIX_FMT_YUV420P;
		auto re = av_frame_get_buffer(frame, 0);
		frame->linesize[0] = w_;
		frame->linesize[1] = w_ / 2;
		frame->linesize[2] = w_ / 2;

		re = sws_scale(rgb2yuv_,
			data,        //输入数据
			line,
			0,
			h_,
			frame->data,
			frame->linesize
		);
		if (re < 0) return;
		cout << "T" << flush;
		//frame->pts = vpts- offset_;

		//Next(frame);

		unique_lock<mutex> lock(mux_);
		if (!next_ && frame->buf[0])
		{
			av_frame_unref(frame_);
			frame_ = frame;
		}
		else
		{
			auto pd = new PData(frame, vpts, offset_);
			Next(pd);
			av_frame_unref(frame);
		}

		this_thread::sleep_for(1ms);
	}
	//清理资源
	screen_.close();
	delete []buf_;
	buf_ = nullptr;
	size_ = 0;

	//if (frame_)
	//{
	//	av_frame_free(&frame_);
	//	frame_ = nullptr;
	//}
	sws_freeContext(rgb2yuv_);
	rgb2yuv_ = nullptr;

	w_ = -1;
	h_ = -1;

	offset_ = 0;
	return;
}



AVFrame* XScreen_get_task::GetFrame()
{
	unique_lock<mutex> lock(mux_);
	if (!frame_ || !frame_->buf[0]) return nullptr;
	auto f = av_frame_alloc();
	f->height = frame_->height;
	f->width = frame_->width;
	auto re = av_frame_ref(f, frame_);   //引用加1
	if (re != 0)
	{
		av_frame_free(&f);
		LOGERROR("XDecodeTask::GetFrame() ERROR");
		return nullptr;
	}
	return f;

}