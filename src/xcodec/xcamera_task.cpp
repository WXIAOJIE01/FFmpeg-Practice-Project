#include "xcamera_task.h"
#include "xvideo_view.h"
#include<iostream>

extern "C" {
#include<libavformat/avformat.h>
#include<libavcodec/avcodec.h>
}
using namespace std;


void XCameraTask::Open()
{
	if (device_name_)
	{
		if (!xc_.Init(device_name_, true))
		{
			cerr << "camera init error" << endl;
			return;
		}
	}
	width_ = xc_.get_width();
	height_ = xc_.get_height();
}

void XCameraTask::Main()
{
	while (!is_exit_)
	{
		auto pkt = xc_.recive_frame();
		long long pts = 0;
		//frame的空间需要下一节点释放
		if (pkt)
		{
			auto frame = xc_.get_frame(pkt);
			pts = NowMs();
			if (frame)
			{
				frame->pts = pts - offset_;
				auto pd = new PData(frame, pts, offset_);
				Next(pd);
				cout << "p" << flush;

				unique_lock<mutex> lock(mux_);
				if (!next_ && frame->buf[0])
				{
					delete pd;
					av_frame_unref(frame_);
					frame_ = frame;
				}
				else
				{
					av_frame_unref(frame);
				}
			}

		}
		this_thread::sleep_for(30ms);
	}
	//清理资源
	if (frame_)
	{
		av_frame_unref(frame_);
		frame_ = nullptr;
	}
	xc_.Close();
	return;
}


AVFrame* XCameraTask::GetFrame()
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

