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

	//���������� 0:camera 1:screen 2:rtsp
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
	std::string save_path_;  //�洢�ĸ�Ŀ¼ 
	long long file_sec_ = 5;   //�����봴��һ���ļ�
	bool is_screen_ = false;    //�Ƿ�����Ļ
	bool is_rtsp_ = false;      //�Ƿ���rtsp
	bool is_camera_ = false;      //�Ƿ���rtmp
	int width_ = 0;
	int height_ = 0;
	int index_ = -1;
	XSynRecord record_;
};

