#pragma once
#include "xtools.h"
#include "xmux.h"
#include "xencode.h"
#include<mutex>
class XCODEC_API XPushRtmpTask :
	public XThread
{
public:
	void Main() override;

	void Do(AVPacket* pkt);

	//�����豸:camera screen file
	bool Open(AVCodecParameters* vxp, AVCodecParameters* axp);

	//��ý��ת��:rtsp
	bool Open(XPara* vxp, XPara* axp);

	bool Open(int rate);

	void Set_url(std::string url) { out_url_ = url; }

	//����Ƶʱ�����
	void set_src_video_time_base(AVRational* tb);
	void set_src_audio_time_base(AVRational* tb);

	void set_src_video_time_base(XRational tb);
	void set_src_audio_time_base(XRational tb);

private:
	std::string out_url_;

	std::mutex mtx_;
	XEncode enc_;
	XMux mux_;
	AVFrame* f_ = nullptr;  //�����洢֡

	AVCodecParameters* enc_par_ = nullptr;

	int width_ = -1;
	int height_ = -1;

	int sampleRate_ = -1;
	int channels_ = -1;

	XAVPacketList pkts_;

	bool is_video = true;

	long long begin_time = 0;

};

