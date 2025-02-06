#include "xpush_rtmp_task.h"
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

void XPushRtmpTask::Main()
{
	while (!is_exit_)
	{
		auto pkt = pkts_.Pop();
		if (!pkt)
		{
			this_thread::sleep_for(1ms);
			continue;
		}
		if (mux_.Write(pkt))
		{
			cout << "su" << flush;
		}
		av_packet_free(&pkt);
	}
	mux_.WriteEnd();

	//清理
	out_url_ = "";
	enc_.free();
	mux_.set_c(nullptr);
	if (f_)
	{
		av_frame_free(&f_);
		f_ = nullptr;
	}
	avcodec_parameters_free(&enc_par_);
	enc_par_ = nullptr;

	width_ = -1;
	height_ = -1;

	sampleRate_ = -1;
	channels_ = -1;

	pkts_.Clear();
	is_video = true;
	begin_time = 0;
	return;
}

void XPushRtmpTask::Do(AVPacket* pkt)
{
	cout << "#" << flush;
	pkts_.Push(pkt);

	return;
}


//默认h264编码，后续支持多种方式
bool XPushRtmpTask::Open(AVCodecParameters* vxp, AVCodecParameters* axp)
{

	auto mux_c = mux_.Open(out_url_.c_str(), vxp, axp, true);
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

bool XPushRtmpTask::Open(XPara* vxp, XPara* axp)
{
	auto mux_c = mux_.Open(out_url_.c_str(), vxp->para, axp->para, true);
	mux_.set_src_video_time_base(vxp->time_base);
	mux_.set_src_audio_time_base(axp->time_base);
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

bool XPushRtmpTask::Open(int rate)
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
	auto mux_c = mux_.Open(out_url_.c_str(), enc_par_, nullptr, true);
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


//音视频时间基础
void XPushRtmpTask::set_src_video_time_base(AVRational* tb)
{
	mux_.set_src_video_time_base(tb);
}
void XPushRtmpTask::set_src_audio_time_base(AVRational* tb)
{
	mux_.set_src_audio_time_base(tb);
}

void XPushRtmpTask::set_src_video_time_base(XRational tb)
{
	AVRational* time = new AVRational();
	time->num = tb.num;
	time->den = tb.num;
	mux_.set_src_video_time_base(time);
}
void XPushRtmpTask::set_src_audio_time_base(XRational tb)
{
	AVRational* time = new AVRational();
	time->num = tb.num;
	time->den = tb.num;
	mux_.set_src_video_time_base(time);
}