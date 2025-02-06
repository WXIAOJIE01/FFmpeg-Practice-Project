#pragma once
#include "xcodec.h"
#include "xtools.h"
class XCODEC_API XDecode:public XCodec
{
public:
	bool Send(const AVPacket* pkt);     //发送编码
	bool Recv(AVFrame* frame);          //获取解码 
	std::vector<AVFrame*> End();        //获取缓存
	bool InitHW(int type = 4);              //初始化硬件加速 AV_HWDEVICE_TYPE_DXVA2
	void SetCopy(bool is_copy) { is_copy_ = is_copy; }

	AVCodecContext* CopyCodecContext() { if (c_) return c_; }

private:
	bool is_copy_ = true;                  //是否复制到内存中渲染
	AVBufferRef* hw_device_ctx = nullptr;  //硬件加速上下文

};

