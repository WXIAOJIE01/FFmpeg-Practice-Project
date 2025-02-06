#pragma once
#include<vector>
#include<iostream>
#include<mutex>
#include "xtools.h"
/// <summary>
/// 编码和解码的基类
/// </summary>



class XCODEC_API XCodec
{
public:
	AVCodecContext* Create(int codec_id, bool is_encode, bool is_video = true);

	/// <summary>
	/// 设置对象的编码器上下文，上下文传递到对象中，资源由XEncode维护
	/// @para c编码器上下文 如果c_不为nullptr，则先清理资源
	/// </summary>
	void set_c(AVCodecContext* c);

	//设置编码参数-重载
	bool Setopt(const char* key, const char* val);
	bool Setopt(const char* key, int val);

	//打开编码器 线程安全
	bool Open();

	//AVCodecContext* CopyPara();
	//AVCodecContext* CopyaudioPara();
	 
	//根据AVCodecContex 创建一个AVFrame，需要调用者释放av_frame_free
	AVFrame* CreateFrame();

	//返回所有编码缓存中AVPacket
	std::vector<AVPacket*> End();

	//释放编码上下文资源
	bool free();

	//清理缓冲资源
	virtual void Clear();

	AVRational* Get_time_base();

protected: 
	AVCodecContext* c_ = nullptr;      //编码器上下文
	std::mutex mtx_;                   //编码器上下文锁

};

