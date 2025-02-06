#pragma once
#include "xtools.h"
#include "xdemux.h"


enum XSYN_TYPE
{
	 XSYN_NONE = 0, //����ͬ��
	 XSYN_VIDEO = 1,  //������Ƶͬ������������Ƶ
};
class XCODEC_API XDemuxTask : public XThread
{
public:
	//���ƽ���
	bool Seek(long long ms);

	int audio_index() { return demux_.audio_index(); }
	int video_index() { return demux_.video_index(); }

	AVPacket* Get_read();

	void Main() override;
	bool Open(std::string url, int timeout_ms=1000);

	//������Ƶ����
	std::shared_ptr<XPara> CopyVideoPara()
	{
		return demux_.CopyVideoPara();
	}

	std::shared_ptr<XPara> CopyAudioPara()
	{
		return demux_.CopyaudioPara();
	}
	//����ͬ������
	void set_syn_type(XSYN_TYPE t) { syn_type_ = t; }

	//������Դ����
	void Stop() override;

	long long get_seek_pts() { return seek_pts_; }
protected: 
	XDemux demux_;
	std::string url_;
	int timeout_ms_ = 0;
	XSYN_TYPE syn_type_ = XSYN_NONE;

	//seek pts;
	long long seek_pts_ = 0;
};

