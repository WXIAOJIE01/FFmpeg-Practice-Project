#pragma once
#include "xtools.h"
#include "xcamera_task.h"
#include "acamera_task.h"
#include "xpush_rtmp_task.h"
#include "xencode_task.h"
#include "xscreen_get_task.h"
#include "xdemux_task.h"
#include "xmux_task.h"
#include <string>
class XCODEC_API XSynRecord :
    public XThread
{
public:
	//�ص���������Ƶ��
	void Do(PData* f) override;

	/// <summary>
	/// ��¼������
	/// </summary>
	/// <param name="url"></param>
	/// <param name="index">0������ͷ�� 1����Ļ�� 2����ý��</param>
	/// <returns></returns>
	bool Open(std::string url, int index, std::string get_url = "");

	//����ͷ�豸
	bool Open1();

	//��Ļ
	bool Open2();

	//ý����
	bool Open3();

	//���߳� ����ͬ��
	void Main() override;
	//���� ���װ ����Ƶ����ʹ���ͬ�����߳�
	void Start() override;

	//�ر��̣߳�����������
	void Stop();

protected:
	std::string url_;         //������ַ
	std::string rtsp_url_;         //����ԭ��ַ
	int index_;               //�������

	XScreen_get_task screen_video_;
	XDemuxTask demux_;
	XCameraTask video_;
	ACameraTask audio_;
	XEncodeTask video_enc_;      //��Ƶ����
	XEncodeTask audio_enc_;      //��Ƶ����
	XMuxTask mux_;         //����Ƶͬ������

	std::mutex mtx_;
};

