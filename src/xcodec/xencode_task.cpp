#include "xencode_task.h"
#include "xencode.h"
#include<iostream>
#include<mutex>
#include<algorithm>
using namespace std;

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")

bool XEncodeTask::Open(int w, int h, int codec_id)
{
	auto par = enc_.Create((AVCodecID)codec_id, true, true);
	if (!par) return false;

	par->width = w;
	par->height = h;

	width_ = w;
	height_ = h;

	//编码器打开
	enc_.set_c(par);

	enc_.Setopt("crf", 18);  //恒定速率因子(CRF)
	enc_.Setopt("tune", "zerolatency");
	if (!enc_.Open())
	{
		cerr << "video encode open error" << endl;
		return false;
	}
	enc_time_base_ = enc_.Get_time_base();
	is_open_ = true;
	return true;
}


bool XEncodeTask::Open(int codec_id)
{
	auto par = enc_.Create((AVCodecID)codec_id, true, false);

	if (!par) return false;

	//编码器打开
	enc_.set_c(par);

	if (!enc_.Open())
	{
		cerr << "audio encode open error" << endl;
		return false;
	}
	enc_time_base_ = enc_.Get_time_base();
	is_open_ = true;
	return true;
}


void XEncodeTask::Do(AVFrame* f)
{
	cout << "#" << flush;
	if (is_video_)  //视频编码任务，排除音频帧
	{
		if (f->width <= 0 || f->height <= 0)
		{
			return;
		}
	}
	else  //音频编码任务，排除视频帧
	{
		if (f->nb_samples <= 0 || f->channels <= 0)
		{
			return;
		}
	}
	frames_.Push(f);
	//av_frame_unref(f);
}

void XEncodeTask::Do(PData* d)
{
	cout << "#" << flush;
	if (!d->data_) return;
	if (is_video_)  //视频编码任务，排除音频帧
	{
		if (d->data_->width <= 0 || d->data_->height <= 0)
		{
			return;
		}
	}
	else  //音频编码任务，排除视频帧
	{
		if (d->data_->nb_samples <= 0 || d->data_->channels <= 0)
		{
			return;
		}
	}
	datas.Push(d);
	//av_frame_unref(f);

}


void XEncodeTask::Main()
{
	while (!is_exit_)
	{
		PData* data = datas.Pop();
		unique_lock<mutex> lock(mux_);
		if (!data||!data->data_)
		{
			this_thread::sleep_for(1ms);
			continue;
		}

		if (!data->data_->buf) continue;
		//防止滤波处理损坏pts
		data->data_->pts = (data->pts_-data->offset_);

		auto pkt_ = enc_.Encode(data->data_);
		if (pkt_)
		{
			pkt_->stream_index = stream_index_;
			pkt_->pts = data->pts_ - data->offset_;
			Next(pkt_);
			cout << "E" << flush;
			this_thread::sleep_for(1ms);
		}
	}
	auto pkts = enc_.End();
	for (auto pkt : pkts)
	{
		av_packet_free(&pkt);
	}

	frames_.Clear();
	datas.Clear();
	enc_.free();

}

void XEncodeTask::Stop()
{
	XThread::Stop();

	cout << "xencode Task free" << endl;
}