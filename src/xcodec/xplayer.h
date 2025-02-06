#pragma once
#include "xtools.h"
#include "xdemux_task.h"
#include "xdecode_task.h"
#include "xvideo_view.h"
#include "xaudio_play.h"

class XCODEC_API XPlayer : public XThread
{
public:
	//�ص���������Ƶ��
	void Do(AVPacket* pkt) override;
	//������Ƶ ��ʼ�����ź���Ⱦ
	bool Open(const char* url, void* winid);
	//���߳� ����ͬ��
	void Main() override;
	//���� ���װ ����Ƶ����ʹ���ͬ�����߳�
	void Start() override; 
	//��Ⱦ��Ƶ������Ƶ
	void Update();
	//�ر��̣߳�����������
	void Stop();
	//���ò����ٶ�
	void SetSpeed(float s);
	//��ʱ�� ms
	long long total_ms() { return total_ms_; }
	//��ǰ���ŵ�λ�� ms
	long long pos_ms() { return pos_ms_; }
	//������Ƶ����λ�� ms
	bool Seek(long long ms);
	//��ͣ���߲���
	void Pause(bool is_pause) override;
	//���ò�������
	void SetVolum(int v);
	//���ý��뷽ʽ
	void Sethw(bool is_hw = false, bool is_copy = true) { video_decode_.Set_hw(is_hw, is_copy); }

	//void Setdevice(void* device) { hw_device_ = device; }
protected:

	void* hw_device_ = nullptr;

	long long total_ms_;
	long long pos_ms_;
	XDemuxTask demux_;				//���װ
	XDecodeTask audio_decode_;		//��Ƶ����
	XDecodeTask video_decode_;		//��Ƶ����
	XVideoView* view_ = nullptr;	//��Ƶ��Ⱦ 

};

