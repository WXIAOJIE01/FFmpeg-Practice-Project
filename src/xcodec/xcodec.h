#pragma once
#include<vector>
#include<iostream>
#include<mutex>
#include "xtools.h"
/// <summary>
/// ����ͽ���Ļ���
/// </summary>



class XCODEC_API XCodec
{
public:
	AVCodecContext* Create(int codec_id, bool is_encode, bool is_video = true);

	/// <summary>
	/// ���ö���ı����������ģ������Ĵ��ݵ������У���Դ��XEncodeά��
	/// @para c������������ ���c_��Ϊnullptr������������Դ
	/// </summary>
	void set_c(AVCodecContext* c);

	//���ñ������-����
	bool Setopt(const char* key, const char* val);
	bool Setopt(const char* key, int val);

	//�򿪱����� �̰߳�ȫ
	bool Open();

	//AVCodecContext* CopyPara();
	//AVCodecContext* CopyaudioPara();
	 
	//����AVCodecContex ����һ��AVFrame����Ҫ�������ͷ�av_frame_free
	AVFrame* CreateFrame();

	//�������б��뻺����AVPacket
	std::vector<AVPacket*> End();

	//�ͷű�����������Դ
	bool free();

	//��������Դ
	virtual void Clear();

	AVRational* Get_time_base();

protected: 
	AVCodecContext* c_ = nullptr;      //������������
	std::mutex mtx_;                   //��������������

};

