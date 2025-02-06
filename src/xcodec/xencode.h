#pragma once
#include "xcodec.h"
class XCODEC_API XEncode:public XCodec
{
public:

	//编码数据 线程安全 每次新创建AVPacket
	AVPacket* Encode(const AVFrame* frame);


	//返回所有编码缓存中AVPacket
	std::vector<AVPacket*> End();

	//释放编码上下文资源
	bool free();

	//复制编码参数
	AVCodecParameters* CopyVideoPara();
 
};

