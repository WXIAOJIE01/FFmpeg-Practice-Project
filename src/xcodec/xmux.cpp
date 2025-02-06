#include "xmux.h"

using namespace std;
extern "C" {
#include<libavformat/avformat.h>
#include<libavutil/opt.h>
}

//预指令导入库
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")

#define BERR(err) if(err!=0){PrintErr(err); return 0; }

AVFormatContext* XMux::Open(const char* url, AVCodecParameters* video_para,
	AVCodecParameters* audio_para, bool is_rtmp)
{
	AVFormatContext* c = nullptr;
	//创建上下文
	if (is_rtmp)
	{
		auto re = avformat_alloc_output_context2(&c, 0, "flv", url);
		if (!c)
		{
			cerr << "output stream create error" << endl;
			return nullptr;
		}
	}
	else
	{
		auto re = avformat_alloc_output_context2(&c, NULL, NULL, url);
		BERR(re);
	}
	//添加视频音频流
	if (video_para)
	{
		auto vs = avformat_new_stream(c, NULL);  //视频流
		avcodec_parameters_copy(vs->codecpar, video_para);
		if (is_rtmp)
		{
			vs->codecpar->codec_tag = 0;
		}
	}
	if (audio_para)
	{
		auto as = avformat_new_stream(c, NULL);  //音频流
		avcodec_parameters_copy(as->codecpar, audio_para);
		if (is_rtmp)
		{
			as->codecpar->codec_tag = 0;
		}
	}


	//打开IO
	auto re = avio_open(&c->pb, url, AVIO_FLAG_WRITE);
	BERR(re);

	return c;

}



bool XMux::writeHead()
{
	unique_lock<mutex> lock(mux_);
	if (!c_) return false;
	auto re = avformat_write_header(c_, nullptr);
	BERR(re);

	//打印输出上下文
	av_dump_format(c_, 0, c_->url, 1);
	this->begin_video_pts = -1;
	this->begin_audio_pts = -1; 
	return true;
}


bool XMux::Write(AVPacket* pkt)
{
	if (!pkt) return false;
	unique_lock<mutex> lock(mux_);
	if (!c_) return false;

	//没有读取到pts 重构考虑通过duration计算
	if (pkt->pts == AV_NOPTS_VALUE)
	{
		pkt->pts = 0;
		pkt->dts = 0;
	}
	if (pkt->stream_index == video_index_)
	{
		cout << "22222" << endl;
		if (begin_video_pts < 0)
			begin_video_pts = pkt->pts;
		lock.unlock();
		RescaleTime(pkt, begin_video_pts, src_video_time_base_);
		lock.lock();

	}
	else if (pkt->stream_index == audio_index_)
	{
		cout << "11111111111" << endl;
		if (begin_audio_pts < 0)
			begin_audio_pts = pkt->pts;
		lock.unlock();
		RescaleTime(pkt, begin_audio_pts, src_audio_time_base_);
		lock.lock();
	}
	cout << pkt->pts << " " << flush;
	//写入一帧数据，内部缓冲排序dts，通过pkt=null 可以写入缓冲 
	auto re = av_interleaved_write_frame(c_, pkt);  //写入排序缓冲
	BERR(re);
	return true;

}

bool XMux::WriteEnd()
{
	unique_lock<mutex> lock(mux_); 
	if (!c_) return false;
	//av_interleaved_write_frame(c_, NULL);  //写入排序缓冲
	auto re = av_write_trailer(c_);
	BERR(re);
	return true;
}


void XMux::set_src_video_time_base(AVRational* tb)
{
	if (!tb) return;
	unique_lock<mutex> lock(mux_);
	if (!src_video_time_base_)
		src_video_time_base_ = new AVRational();
	*src_video_time_base_ = *tb;
}
void XMux::set_src_audio_time_base(AVRational* tb)
{
	if (!tb) return;
	unique_lock<mutex> lock(mux_);
	if (!src_audio_time_base_)
		src_audio_time_base_ = new AVRational();
	*src_audio_time_base_ = *tb;
}
 
XMux::~XMux()
{
	unique_lock<mutex> lock(mux_);
	if (src_video_time_base_)
		delete src_video_time_base_;
	src_video_time_base_ = nullptr;
	if (src_audio_time_base_)
		delete src_audio_time_base_;
	src_audio_time_base_ = nullptr;
}