#include "xsyn_push.h"
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

void XSynPush::Do(PData* f)
{
	unique_lock<mutex> lock(mtx_);
	if (!f->data_) return;
	if(f->data_->width>0||f->data_->height>0)
		video_filter_.Do(f);
	else
		audio_filter_.Do(f);
}

bool XSynPush::Open(std::string url, int index, std::string get_url)
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
bool XSynPush::Open1()
{
	video_filter_.set_stream_index(0);
	audio_filter_.set_stream_index(1);

	video_filter_.Open(filtertype_);
	audio_filter_.Open(filtertype_);


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

	push_.Set_url(url_);
	if (!push_.Open(vpar, apar))
	{
		cerr << "open push server error" << endl;
		return false;
	}

	video_.set_next(this);
	audio_.set_next(this);

	video_filter_.set_next(&video_enc_);
	audio_filter_.set_next(&audio_enc_);

	video_enc_.set_next(&push_);
	audio_enc_.set_next(&push_);

	video_enc_.set_stream_index(0);
	audio_enc_.set_stream_index(1);

	return true;
}


//屏幕
bool XSynPush::Open2()
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

	push_.Set_url(url_);
	if (!push_.Open(vpar, apar))
	{
		cerr << "open push server error" << endl;
		return false;
	}

	screen_video_.set_next(this);
	audio_.set_next(this);

	video_enc_.set_next(&push_);
	audio_enc_.set_next(&push_);

	video_enc_.set_stream_index(0);
	audio_enc_.set_stream_index(1);

	return true;
}


//流媒体
bool XSynPush::Open3()
{
	if (!demux_.Open(rtsp_url_))
	{
		cerr << "open rtsp url error" << endl;
		return false;
	}
	auto vpar = demux_.CopyVideoPara();
	auto apar = demux_.CopyAudioPara();


	push_.Set_url(url_);
	if (!push_.Open(vpar.get(), apar.get()))
	{
		cerr << "open push server error" << endl;
		return false;
	}
	demux_.set_next(&push_);
	return true;
}

//主线程 处理同步
void XSynPush::Main()
{

}
//开启 解封装 音视频解码和处理同步的线程
void XSynPush::Start()
{
	if (index_ == 0)
	{
		video_filter_.Start();
		audio_filter_.Start();
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
	//XThread::Start();
	push_.Start();
}

//关闭线程，进行清理工作
void XSynPush::Stop()
{
	screen_video_.Stop();
	demux_.Stop();
	video_.Stop();
	audio_.Stop();
	video_enc_.Stop();      //视频编码
	audio_enc_.Stop();      //音频编码
	video_filter_.Stop();
	audio_filter_.Stop();
	audio_.Stop();
	audio_.Stop();
	push_.Stop();         //音视频同步推流
}