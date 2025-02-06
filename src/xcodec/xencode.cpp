#include "xencode.h"
#include<iostream>
using namespace std;

extern "C" {
#include<libavcodec/avcodec.h>
#include<libavutil/opt.h>
}

//预指令导入库
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avutil.lib")




AVPacket* XEncode::Encode(const AVFrame* frame)
{
	if (!frame) return nullptr;
	unique_lock<mutex>lock(mtx_);
	if (!c_) return nullptr;

	//发送到编码线程
	auto re = avcodec_send_frame(c_, frame);
	if (re != 0) return nullptr;
	auto pkt = av_packet_alloc();

	//接收编码线程数据
	re = avcodec_receive_packet(c_, pkt);
	if (re == 0)
	{
		return pkt;
	}
	av_packet_free(&pkt);
	if (re == AVERROR(EAGAIN) || re == AVERROR_EOF)
	{
		return nullptr;
	}
	if (re < 0)
	{
		PrintErr(re);
	}
	return nullptr;
}



AVRational* XCodec::Get_time_base() {
	return &(c_->time_base);
}


bool XEncode::free()
{
	unique_lock<mutex>lock(mtx_);
	if (c_)
	{
		avcodec_free_context(&c_);
		c_ = nullptr;
	}
	return true;
}

AVCodecParameters* XEncode::CopyVideoPara()
{
	unique_lock<mutex>lock(mtx_);
	AVCodecParameters* vp = avcodec_parameters_alloc();
	if (c_)
	{
		avcodec_parameters_from_context(vp, c_);
	}
	else
	{
		return NULL;
	}
	return vp;
}


vector<AVPacket*> XEncode::End()
{
	std::vector<AVPacket*> res;
	unique_lock<mutex>lock(mtx_);
	if (!c_) return res;
	cout << "=================encode_task_1==============" << endl;
	auto re = avcodec_send_frame(c_, NULL);   //发送NULL， 获取缓冲
	if (re != 0) return res;
	while (re >= 0)
	{
		auto pkt = av_packet_alloc();
		re = avcodec_receive_packet(c_, pkt);
		if (re != 0)
		{
			av_packet_free(&pkt);
			break;
		}
		res.push_back(pkt);
	}
	return res;

}