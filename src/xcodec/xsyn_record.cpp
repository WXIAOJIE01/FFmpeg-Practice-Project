#include "xsyn_record.h"

#include<iostream>
using namespace std;



extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")

void XSynRecord::Do(PData* f)
{
	unique_lock<mutex> lock(mtx_);
	if (video_enc_.is_open())
	{
		video_enc_.Do(f);
	}
	if (audio_enc_.is_open())
	{
		audio_enc_.Do(f);
	}
}

bool XSynRecord::Open(std::string url, int index, std::string get_url)
{
	if (url.empty() || index < 0 || index>2)
	{
		cerr << "open syn push task error" << endl;
		return false;
	}
	url_ = url;
	index_ = index;
	if (!get_url.empty()) rtsp_url_ = get_url;
	if (index == 0)
	{
		Open1();
	}
	else if (index == 1)
	{
		Open2();
	}
	else if (index == 2)
	{
		Open3();
	}
	return true;
}

//摄像头
bool XSynRecord::Open1()
{
	video_.Set_device("video=HIK 1080P Camera");
	video_.Open();
	if (!audio_.Open())
	{
		cerr << "open audio error" << endl;
		return false;
	}

	int w = video_.get_width();
	int h = video_.get_height();
	video_enc_.Open(w, h, AV_CODEC_ID_H264);
	video_enc_.Set_index(true);

	audio_enc_.Open(AV_CODEC_ID_AAC);
	audio_enc_.Set_index(false);

	long long begin_time = NowMs();
	video_.Set_offset(begin_time);
	audio_.Set_offset(begin_time);

	auto vpar = video_enc_.GetvideoPara();
	auto apar = audio_enc_.GetvideoPara();

	auto video_time_base = video_enc_.get_time_base();
	auto audio_time_base = audio_enc_.get_time_base();

	if (!mux_.Open(url_.c_str(), vpar, video_time_base, apar, audio_time_base))
	{
		cerr << "open mux_ server error" << endl;
		return false;
	}

	video_.set_next(this);
	audio_.set_next(this);

	video_enc_.set_next(&mux_);
	audio_enc_.set_next(&mux_);

	video_enc_.set_stream_index(0);
	audio_enc_.set_stream_index(1);

	return true;
}


//屏幕
bool XSynRecord::Open2()
{
	screen_video_.Open();
	if (!audio_.Open())
	{
		cerr << "open audio error" << endl;
		return false;
	}

	int w = screen_video_.get_width();
	int h = screen_video_.get_height();
	video_enc_.Open(w, h, AV_CODEC_ID_H264);
	video_enc_.Set_index(true);

	audio_enc_.Open(AV_CODEC_ID_AAC);
	audio_enc_.Set_index(false);

	long long begin_time = NowMs();
	screen_video_.Set_offset(begin_time);
	audio_.Set_offset(begin_time);

	auto vpar = video_enc_.GetvideoPara();
	auto apar = audio_enc_.GetvideoPara();

	auto video_time_base = video_enc_.get_time_base();
	auto audio_time_base = audio_enc_.get_time_base();

	if (!mux_.Open(url_.c_str(), vpar, video_time_base, apar, audio_time_base))
	{
		cerr << "open mux_ server error" << endl;
		return false;
	}

	screen_video_.set_next(this);
	audio_.set_next(this);

	video_enc_.set_next(&mux_);
	audio_enc_.set_next(&mux_);

	video_enc_.set_stream_index(0);
	audio_enc_.set_stream_index(1);

	return true;
}


//流媒体
bool XSynRecord::Open3()
{
	if (!demux_.Open(rtsp_url_))
	{
		cerr << "open rtsp url error" << endl;
		return false;
	}
	auto vpar = demux_.CopyVideoPara();
	auto apar = demux_.CopyAudioPara();

	auto video_time_base = vpar->time_base;
	auto audio_time_base = apar->time_base;

	if (!mux_.Open(url_.c_str(), vpar->para, video_time_base, apar->para, audio_time_base))
	{
		cerr << "open mux_ server error" << endl;
		return false;
	}
	demux_.set_next(&mux_);
	return true;
}

//主线程 处理同步
void XSynRecord::Main()
{

}
//开启 解封装 音视频解码和处理同步的线程
void XSynRecord::Start()
{
	if (index_ == 0)
	{
		video_.Start();
		audio_.Start();
		if (video_enc_.is_open())
		{
			video_enc_.Start();
		}
		if (audio_enc_.is_open())
		{
			audio_enc_.Start();
		}
	}
	else if (index_ == 1)
	{
		screen_video_.Start();
		audio_.Start();
		if (video_enc_.is_open())
		{
			video_enc_.Start();
		}
		if (audio_enc_.is_open())
		{
			audio_enc_.Start();
		}
	}
	else if (index_ == 2)
	{
		demux_.Start();
	}
	XThread::Start();
	mux_.Start();
}

//关闭线程，进行清理工作
void XSynRecord::Stop()
{
	mux_.Stop();
}
