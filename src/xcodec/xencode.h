#pragma once
#include "xcodec.h"
class XCODEC_API XEncode:public XCodec
{
public:

	//�������� �̰߳�ȫ ÿ���´���AVPacket
	AVPacket* Encode(const AVFrame* frame);


	//�������б��뻺����AVPacket
	std::vector<AVPacket*> End();

	//�ͷű�����������Դ
	bool free();

	//���Ʊ������
	AVCodecParameters* CopyVideoPara();
 
};

