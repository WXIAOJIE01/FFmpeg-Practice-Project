#include "xcodec.h"
using namespace std;

extern "C" {
#include<libavcodec/avcodec.h>
#include<libavutil/opt.h>
}

//预指令导入库
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")


/// <summary>
/// 创建编码器上下文
/// </summary>
/// <param name="codec_id"></编码器ID号，对应ffmpeg>
/// <returns></returns>编码上下文，失败返回nullptr
AVCodecContext* XCodec::Create(int codec_id, bool is_encode, bool is_video)
{
	//1 找到编码器
	AVCodec* codec = nullptr;
	if (is_encode)
		codec = avcodec_find_encoder((AVCodecID)codec_id);
	else
		codec = avcodec_find_decoder((AVCodecID)codec_id);
	if (!codec)
	{
		cerr << "avcodec_find_encode error: " << codec_id << endl;
		return nullptr;
	}

	//2 创建上下文
	auto c = avcodec_alloc_context3(codec);
	if (!c)
	{
		cerr << "avcodec_alloc_context3: " << codec_id << endl;
		return nullptr;
	}

	if (is_video)
	{
		//3 设置参数默认值
		c->time_base = { 1, 25 };
		c->pix_fmt = AV_PIX_FMT_YUV420P;
		c->thread_count = 16;
		c->time_base = { 1, 1000 };
		c->skip_frame = AVDISCARD_DEFAULT;
	}
	else
	{
		c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		c->thread_count = 16;
		c->bit_rate = 40000;
		c->sample_fmt = AV_SAMPLE_FMT_FLTP;
		c->channels = 2;
		c->sample_rate = 44100;
		c->channel_layout = av_get_default_channel_layout(2);
		c->time_base = { 1, 1000 };
	}

	return c;

}

void XCodec::set_c(AVCodecContext* c)
{
	unique_lock<mutex>lock(mtx_);
	if (c_)
	{
		if (c_->hw_device_ctx)
			av_buffer_unref(&(c_->hw_device_ctx));
		avcodec_free_context(&c_);
	}
	this->c_ = c;
}


bool XCodec::Setopt(const char* key, const char* val)
{
	unique_lock<mutex>lock(mtx_);
	if (!c_) return false;
	auto re = av_opt_set(c_->priv_data, key, val, 0);
	if (re != 0)
	{
		cerr << "set opt error" << endl;
		PrintErr(re);
	}
	return true;
}
bool XCodec::Setopt(const char* key, int val)
{
	unique_lock<mutex>lock(mtx_);
	if (!c_) return false;
	auto re = av_opt_set_int(c_->priv_data, key, val, 0);
	if (re != 0)
	{
		cerr << "set opt error" << endl;
		PrintErr(re);
	}
	return true;
}


bool XCodec::Open()
{
	unique_lock<mutex>lock(mtx_);
	if (!c_) return false;
	auto re = avcodec_open2(c_, NULL, NULL);
	if (re != 0)
	{
		cout << "avcodec open error" << endl;
		PrintErr(re);
		return false;
	}
	return true;
}




AVFrame* XCodec::CreateFrame()
{
	unique_lock<mutex>lock(mtx_);
	if (!c_) return nullptr;
	auto frame = av_frame_alloc();
	frame->width = c_->width;
	frame->height = c_->height;
	frame->format = c_->pix_fmt;
	auto re = av_frame_get_buffer(frame, 0);
	if (re != 0)
	{
		av_frame_free(&frame);
		PrintErr(re);
		return nullptr;
	}
	return frame;
}

void XCodec::Clear()
{
	unique_lock<mutex>lock(mtx_);
	if (!c_) return;
	avcodec_flush_buffers(c_);
}


