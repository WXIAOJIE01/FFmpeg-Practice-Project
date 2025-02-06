#pragma once
#include "xformat.h"
class XCODEC_API XDemux:public XFormat
{
public:
	/// <summary>
	/// 打开解封装
	/// </summary>
	/// <param name="url"></param>解封装地址 支持rtsp
	/// <returns></returns>失败返回nullptr
	static AVFormatContext* Open(const char* url);

	/// <summary>
	/// 读取一帧数据
	/// </summary>
	/// <param name="pkt">输出数据</param>
	/// <returns>是否成功</returns>
	bool Read(AVPacket* pkt);

	bool Seek(long long pts, int stream_index);

	
};

