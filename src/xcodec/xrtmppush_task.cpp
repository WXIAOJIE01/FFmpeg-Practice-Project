#include "xrtmppush_task.h"
#include<iostream>
#include "xvideo_view.h"
using namespace std;

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")

void XRtmppushTask::Main()
{
	int vpts = 0;
	int apts = 0;

	//音频pts的计算
	int add = av_rescale_q(1024, { 1, sampleRate_ }, { sampleRate_, 1 });
	//int add = av_rescale_q(pcm_->nb_samples, { 1, sampleRate_ }, enc_par_->time_base);

	while (!is_exit_)
	{
		auto frame = frames_.Pop();
		if (!frame)
		{
			this_thread::sleep_for(1ms);
			continue;
		}
		if (is_video)
		{
			frame->pts = vpts;
			vpts++;
		}
		else
		{
			frame->pts = vpts;
			vpts+=add;
		}

		auto pkt = enc_.Encode(frame);
		if (pkt)
		{
			if (mux_.Write(pkt))
			{
				cout << "su" << flush;
			}
			av_packet_unref(pkt);
		}
	}
	mux_.WriteEnd();
}

void XRtmppushTask::Do(AVFrame* f)
{
	cout << "#" << flush;
	frames_.Push(f);

	return;
}

//默认h264编码，后续支持多种方式
bool XRtmppushTask::Open(const char* url_, int w, int h)
{
	avformat_network_init();
	width_ = w;
	height_ = h;
	auto par = enc_.Create(AV_CODEC_ID_H264, true);

	if (!par) return false;

	par->width = width_;
	par->height = height_;
	par->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	par->framerate = { 25,1 };

	//画面组的大小，多少帧一个关键帧
	par->gop_size = 50;
	par->max_b_frames = 0;

	//编码器打开
	enc_.set_c(par);
	enc_.Setopt("crf", 18);  //恒定速率因子(CRF)
	enc_.Setopt("tune", "zerolatency");
	if (!enc_.Open())
	{
		cerr << "encode open error" << endl;
		return false;
	}

	enc_par_ = enc_.CopyVideoPara();
	out_url_ = url_;
	auto mux_c = mux_.Open(url_, enc_par_, nullptr, true);
	if (!mux_c)
	{
		cerr << "mux open error" << endl;
		return false;
	}
	mux_.set_c(mux_c);

	if (!mux_.writeHead())
	{
		cerr << "write head error" << endl;
		return false;
	}

	return true;
}



bool XRtmppushTask::Open(const char* url_, int rate)
{
	avformat_network_init();
	sampleRate_ = rate;
	channels_ = 2;
	is_video = false;

	auto apar = enc_.Create(AV_CODEC_ID_AAC, true, false);
	apar->sample_rate = sampleRate_;
	apar->channels = channels_;
	apar->channel_layout = av_get_default_channel_layout(channels_);

	enc_.set_c(apar);
	if (!enc_.Open())
	{
		cerr << "encode open error" << endl;
		return false;
	}

	//音频参数也可以用视频的接口复制
	enc_par_ = enc_.CopyVideoPara();
	out_url_ = url_;
	auto mux_c = mux_.Open(url_, enc_par_, nullptr, true);
	if (!mux_c)
	{
		cerr << "mux open error" << endl;
		return false;
	}
	mux_.set_c(mux_c);

	if (!mux_.writeHead())
	{
		cerr << "write head error" << endl;
		return false;
	}

	return true;
}