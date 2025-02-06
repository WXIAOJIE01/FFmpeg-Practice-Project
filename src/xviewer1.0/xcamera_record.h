#pragma once
#include<string>
#include "xtools.h"
#include "xsyn_record.h"
class XCameraRecord : public XThread
{
public:
	//rtsp
	void set_rtsp_url(std::string url) { rtsp_url_ = url; }
	void set_save_path(std::string s) { save_path_ = s; }
	void set_file_sec(int s) { file_sec_ = s; }

	//设置流类型 0:camera 1:screen 2:rtsp
	void set_type(int index)
	{
		index_ = index;
		switch (index)
		{
		case 1:
			is_screen_ = true;
		case 2:
			is_rtsp_ = true;
		case 0:
			is_camera_ = true;
		default:
			break;
		}
	}
private:
	void Main() override; 
	std::string rtsp_url_;
	std::string save_path_;  //存储的根目录 
	long long file_sec_ = 5;   //多少秒创建一个文件
	bool is_screen_ = false;    //是否是屏幕
	bool is_rtsp_ = false;      //是否是rtsp
	bool is_camera_ = false;      //是否是rtmp
	int width_ = 0;
	int height_ = 0;
	int index_ = -1;
	XSynRecord record_;
};

