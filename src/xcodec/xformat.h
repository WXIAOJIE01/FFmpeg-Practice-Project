   #pragma once

/// <summary>
/// 封装和解封装基类
/// </summary>
#include<mutex>
#include<iostream>
#include "xtools.h"


struct XRational
{
	int num;     //Numerator
	int den;     //Denominator
};




class XCODEC_API XFormat
{
public:
	bool CopyPara(int stream_index, AVCodecContext* dst);
	/// <summary>
	/// 复制参数 线程安全
	/// </summary>
	/// <param name="stream_index">对应c->streams下标</param>
	/// <param name="dst">输出参数</param>
	/// <returns>是否成功</returns>
	bool CopyPara(int stream_index, AVCodecParameters* dst);
	/// <summary>
	/// 设置上下文，并清理上次设置的值，如果传递NULL，相当于关闭上下文 线程安全
	/// </summary>
	/// <param name="c"></param>
	/// 
	/// 
	//返回智能指针
	std::shared_ptr<XPara> CopyVideoPara();
	std::shared_ptr<XPara> CopyaudioPara();


	void set_c(AVFormatContext* c);
	int audio_index() { return audio_index_; }
	int video_index() { return video_index_; }
	XRational video_time_base(){ return video_time_base_; }
	XRational audio_time_base() { return audio_time_base_; }

	//根据timebase换算时间
	bool RescaleTime(AVPacket *pkt, long long offset_pts, XRational time_base);
	bool RescaleTime(AVPacket* pkt, long long offset_pts, AVRational* time_base);

	//把pts dts duration值转为毫秒
	long long RescaleToMs(long long pts, int index); 
 
	int video_codec_id() { return video_codec_id_; }
	
	//判断是否超时
	bool IsTimeout()
	{
		if (NowMs() - last_time_ > time_out_ms_) //超时
		{
			last_time_ = NowMs();
			is_conneted_ = false; 
			return true;
		}
			
		return false;
	}

	//设定超时时间
	void set_time_out_ms(int ms);

	bool is_conneted() { return is_conneted_; }
protected:

	int time_out_ms_ = 0;          //超时时间 毫秒
	long long last_time_ = 0;         //上次接受到
	bool is_conneted_ = false;        //是否连接成功
	

	AVFormatContext* c_ = nullptr;     //封装解封装上下文
	std::mutex mux_;                   //c_资源互斥
	int video_index_ = 0; //video和audio在stream中的索引
	int audio_index_ = 1;
	XRational video_time_base_ = { 1, 25 };
	XRational audio_time_base_ = { 1, 9000 };
	int video_codec_id_ = 0;         //编码器ID

};

