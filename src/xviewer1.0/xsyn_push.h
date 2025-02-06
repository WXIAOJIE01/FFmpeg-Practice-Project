#pragma once
#include "xtools.h"
#include "xcamera_task.h"
#include "acamera_task.h"
#include "xpush_rtmp_task.h"
#include "xencode_task.h"
#include "xscreen_get_task.h"
#include "xdemux_task.h"
#include "xfilter_task.h"
#include <string>

//�����̣߳�����Ƶ�������Ƶ���봦����ͬ��
class XSynPush : public XThread
{
public:
	//�ص���������Ƶ��
	void Do(PData* f) override;

	/// <summary>
	/// ����������
	/// </summary>
	/// <param name="url"></param>
	/// <param name="index">0������ͷ�� 1����Ļ�� 2����ý��ת��</param>
	/// <returns></returns>
	bool Open(std::string url, int index, std::string get_url = "");

	//����ͷ�豸 ��ʼ�����ź���Ⱦ
	bool Open1();

	//��Ļ
	bool Open2();

	//ý����
	bool Open3();

	//���߳� ����ͬ��
	void Main() override;
	//���� ���װ ����Ƶ����ʹ���ͬ�����߳�
	void Start() override;

	void SetFilter(FilterType type) { filtertype_ = type; }

	//�ر��̣߳�����������
	void Stop();

protected:
	FilterType filtertype_ = raw;
	std::string url_;         //������ַ
	std::string rtsp_url_;         //����ԭ��ַ
	int index_;               //�������

	XScreen_get_task screen_video_;
	XDemuxTask demux_;
	XCameraTask video_;
	ACameraTask audio_;
	XEncodeTask video_enc_;      //��Ƶ����
	XEncodeTask audio_enc_;      //��Ƶ����
	XPushRtmpTask push_;         //����Ƶͬ������
	XFilterTask video_filter_;   //��Ƶ�˾��㷨
	XFilterTask audio_filter_;   //��Ƶ�˾��㷨

	std::mutex mtx_;


};

