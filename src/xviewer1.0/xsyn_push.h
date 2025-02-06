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

//推流线程，在视频编码和音频编码处进行同步
class XSynPush : public XThread
{
public:
	//回调接收音视频包
	void Do(PData* f) override;

	/// <summary>
	/// 打开推流任务
	/// </summary>
	/// <param name="url"></param>
	/// <param name="index">0：摄像头， 1：屏幕， 2：流媒体转发</param>
	/// <returns></returns>
	bool Open(std::string url, int index, std::string get_url = "");

	//摄像头设备 初始化播放和渲染
	bool Open1();

	//屏幕
	bool Open2();

	//媒体流
	bool Open3();

	//主线程 处理同步
	void Main() override;
	//开启 解封装 音视频解码和处理同步的线程
	void Start() override;

	void SetFilter(FilterType type) { filtertype_ = type; }

	//关闭线程，进行清理工作
	void Stop();

protected:
	FilterType filtertype_ = raw;
	std::string url_;         //推流地址
	std::string rtsp_url_;         //拉流原地址
	int index_;               //任务序号

	XScreen_get_task screen_video_;
	XDemuxTask demux_;
	XCameraTask video_;
	ACameraTask audio_;
	XEncodeTask video_enc_;      //视频编码
	XEncodeTask audio_enc_;      //音频编码
	XPushRtmpTask push_;         //音视频同步推流
	XFilterTask video_filter_;   //视频滤镜算法
	XFilterTask audio_filter_;   //视频滤镜算法

	std::mutex mtx_;


};

