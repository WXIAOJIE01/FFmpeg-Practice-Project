#pragma once
#include "xcodec.h"
#include "xtools.h"
class XCODEC_API XDecode:public XCodec
{
public:
	bool Send(const AVPacket* pkt);     //���ͱ���
	bool Recv(AVFrame* frame);          //��ȡ���� 
	std::vector<AVFrame*> End();        //��ȡ����
	bool InitHW(int type = 4);              //��ʼ��Ӳ������ AV_HWDEVICE_TYPE_DXVA2
	void SetCopy(bool is_copy) { is_copy_ = is_copy; }

	AVCodecContext* CopyCodecContext() { if (c_) return c_; }

private:
	bool is_copy_ = true;                  //�Ƿ��Ƶ��ڴ�����Ⱦ
	AVBufferRef* hw_device_ctx = nullptr;  //Ӳ������������

};

