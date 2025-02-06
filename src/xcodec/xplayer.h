#pragma once
#include "xtools.h"
#include "xdemux_task.h"
#include "xdecode_task.h"
#include "xvideo_view.h"
#include "xaudio_play.h"

class XCODEC_API XPlayer : public XThread
{
public:
	//回调接收音视频包
	void Do(AVPacket* pkt) override;
	//打开音视频 初始化播放和渲染
	bool Open(const char* url, void* winid);
	//主线程 处理同步
	void Main() override;
	//开启 解封装 音视频解码和处理同步的线程
	void Start() override; 
	//渲染视频播放音频
	void Update();
	//关闭线程，进行清理工作
	void Stop();
	//设置播放速度
	void SetSpeed(float s);
	//总时长 ms
	long long total_ms() { return total_ms_; }
	//当前播放的位置 ms
	long long pos_ms() { return pos_ms_; }
	//设置视频播放位置 ms
	bool Seek(long long ms);
	//暂停或者播放
	void Pause(bool is_pause) override;
	//设置播放音量
	void SetVolum(int v);
	//设置解码方式
	void Sethw(bool is_hw = false, bool is_copy = true) { video_decode_.Set_hw(is_hw, is_copy); }

	//void Setdevice(void* device) { hw_device_ = device; }
protected:

	void* hw_device_ = nullptr;

	long long total_ms_;
	long long pos_ms_;
	XDemuxTask demux_;				//解封装
	XDecodeTask audio_decode_;		//音频解码
	XDecodeTask video_decode_;		//视频解码
	XVideoView* view_ = nullptr;	//视频渲染 

};

