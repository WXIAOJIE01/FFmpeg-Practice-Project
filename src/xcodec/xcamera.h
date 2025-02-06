#pragma once
#include <mutex>
#include"xtools.h"
struct AVFormatContext;
struct AVInputFormat;
struct SwsContext;
struct AVFrame;
struct AVPacket;
struct SwrContext;

class XCODEC_API XCamera
{
public:
	bool Init(const char* name, bool is_video = true);
	bool Init_video();

	AVPacket* recive_frame();

	AVFrame* get_frame(AVPacket* pkt);

	int get_width() { return width_; }
	int get_height() { return height_; }

	void Close();

private:
	AVFormatContext* inputFormatContext_ = nullptr;
	AVInputFormat* inputformat_ = nullptr;
	const char* device_name_ = nullptr;

	int videoStreamIndex_ = -1;
	int audioStreamIndex_ = -1;

	int format_index_ = -1;
	SwsContext* yuyv2yuv_ = nullptr;

	int width_ = 0;
	int height_ = 0;

	AVFrame* frame_ = nullptr;
	std::mutex mux_;


};

