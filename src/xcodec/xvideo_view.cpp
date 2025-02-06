#include "xsdl.h"
#include "xtools.h"
#include <thread>
#include <iostream>
using namespace std;
extern "C"
{
#include <libavcodec/avcodec.h>
}
#pragma comment(lib,"avutil.lib")

bool XVideoView::Init(AVCodecParameters* para)
{
	if (!para)return false;
	auto fmt = (Format)para->format;
	switch (para->format)
	{
	case AV_PIX_FMT_YUV420P:
	case AV_PIX_FMT_YUVJ420P:
		fmt = YUV420P;
		break;
	default:
		break;
	}
	return Init(para->width, para->height, fmt);
}

AVFrame* XVideoView::Read()
{
	if (width_ <= 0 || height_ <= 0 || !ifs_)return NULL;
	//AVFrame�ռ��Ѿ����룬������������仯����Ҫ�ͷſռ�
	if (frame_)
	{
		if (frame_->width != width_
			|| frame_->height != height_
			|| frame_->format != fmt_)
		{
			//�ͷ�AVFrame����ռ䣬��buf���ü�����һ
			av_frame_free(&frame_);
		}
	}
	if (!frame_)
	{
		//�������ռ�����ؿռ�
		frame_ = av_frame_alloc();
		frame_->width = width_;
		frame_->height = height_;
		frame_->format = fmt_;
		frame_->linesize[0] = width_ * 4;
		if (frame_->format == AV_PIX_FMT_YUV420P)
		{
			frame_->linesize[0] = width_; // Y
			frame_->linesize[1] = width_ / 2;//U
			frame_->linesize[2] = width_ / 2;//V
		}

		//����AVFrame�ռ䣬ʹ��Ĭ�϶���
		auto re = av_frame_get_buffer(frame_, 0);
		if (re != 0)
		{
			char buf[1024] = { 0 };
			av_strerror(re, buf, sizeof(buf) - 1);
			cout << buf << endl;
			av_frame_free(&frame_);
			return NULL;
		}
	}
	if (!frame_)return NULL;

	//��ȡһ֡����
	if (frame_->format == AV_PIX_FMT_YUV420P)
	{
		ifs_.read((char*)frame_->data[0],
			frame_->linesize[0] * height_);	//Y
		ifs_.read((char*)frame_->data[1],
			frame_->linesize[1] * height_ / 2);	//U
		ifs_.read((char*)frame_->data[2],
			frame_->linesize[2] * height_ / 2);	//V
	}
	else	//RGBA ARGB BGRA 32
	{
		ifs_.read((char*)frame_->data[0], frame_->linesize[0] * height_);
	}
	if (ifs_.gcount() == 0)
		return NULL;
	return frame_;
}

//���ļ�
bool XVideoView::Open(std::string filepath)
{
	if (ifs_.is_open())
	{
		ifs_.close();
	}
	ifs_.open(filepath, ios::binary);
	return ifs_.is_open();
}
XVideoView* XVideoView::Create(RenderType type)
{
	switch (type)
	{
	case XVideoView::SDL:
		return new XSDL();
		break;
	default:
		break;
	}
	return nullptr;
}
XVideoView::~XVideoView()
{
	if (cache_)
		delete cache_;
	cache_ = nullptr;
}
bool XVideoView::DrawFrame(AVFrame* frame)
{
	if (!frame || !frame->data[0])return false;
	count_++;
	if (beg_ms_ <= 0)
	{
		beg_ms_ = clock();
	}
	//������ʾ֡��
	else if ((clock() - beg_ms_) / (CLOCKS_PER_SEC / 1000) >= 1000) //һ�����һ��fps
	{
		render_fps_ = count_;
		count_ = 0;
		beg_ms_ = clock();
	}
	int linesize = 0;

	int width = 0;
	int height = 0;
	unsigned char* y_plane = nullptr;
	unsigned char* u_plane = nullptr;
	unsigned char* v_plane = nullptr;

	switch (frame->format)
	{
	case AV_PIX_FMT_YUV420P:
	case AV_PIX_FMT_YUVJ420P:
		return Draw(frame->data[0], frame->linesize[0],//Y
			frame->data[1], frame->linesize[1],	//U
			frame->data[2], frame->linesize[2]	//V
		);
	case AV_PIX_FMT_NV12:
		width = frame->width;
		height = frame->height;

		if (!cache_) {
			// Ϊ YUV420p �����㹻���ڴ�
			cache_ = new unsigned char[width * height * 3 / 2];
		}

		y_plane = cache_;
		u_plane = cache_ + width * height;
		v_plane = u_plane + (width / 2) * (height / 2);

		// ���� Y ƽ��
		if (frame->linesize[0] == width) {
			memcpy(y_plane, frame->data[0], width * height);
		}
		else {
			for (int i = 0; i < height; i++) {
				memcpy(y_plane + i * width, frame->data[0] + i * frame->linesize[0], width);
			}
		}

		// ת�� UV ƽ��
		if (frame->linesize[1] == width) {
			for (int i = 0; i < height / 2; i++) {
				for (int j = 0; j < width / 2; j++) {
					u_plane[i * (width / 2) + j] = frame->data[1][i * width + 2 * j];
					v_plane[i * (width / 2) + j] = frame->data[1][i * width + 2 * j + 1];
				}
			}
		}
		else {
			for (int i = 0; i < height / 2; i++) {
				for (int j = 0; j < width / 2; j++) {
					u_plane[i * (width / 2) + j] = frame->data[1][i * frame->linesize[1] + 2 * j];
					v_plane[i * (width / 2) + j] = frame->data[1][i * frame->linesize[1] + 2 * j + 1];
				}
			}
		}

		// ��ת����� YUV420p ���ݴ��ݸ���Ⱦ����
		return Draw(cache_, width);
	case AV_PIX_FMT_BGRA:
	case AV_PIX_FMT_ARGB:
	case AV_PIX_FMT_RGBA:
		return Draw(frame->data[0], frame->linesize[0]);
	default:
		break;
	}
	return false;
}