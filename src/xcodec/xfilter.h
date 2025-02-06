#pragma once
#include "xtools.h"

//��������ͷ�ļ�
namespace cv
{
	class Mat;
}

class XCODEC_API XFilter
{
public:
	static XFilter* create(FilterType type);

	virtual bool Init() = 0;

	virtual PData* process(PData* frame) = 0;

	cv::Mat AVFrameToCVMat(AVFrame* frame);

	AVFrame* CVMatToAVFrame(cv::Mat& inMat);

	void set_stream_index(int index) { stream_index_ = index; }
protected:
	//-1:δ��ʼ�� 0����Ƶ 1����Ƶ
	int stream_index_ = -1;

};



