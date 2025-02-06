#include "xscreen_get_task.h"
#include <d3d9.h>
#include<iostream>
using namespace std;

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")

#pragma comment(lib, "d3d9.lib")

void XScreen_get_task::Open()
{
	if (!screen_.Init())
	{
		cerr << "directX init error" << endl;
		return;
	}
	w_ = screen_.get_width();
	h_ = screen_.get_height();

	size_ = w_ * h_ * 4;
	buf_ = new unsigned char[size_];


	//RGB->yuv��
	rgb2yuv_ = sws_getCachedContext(
		rgb2yuv_,				//ת�������ģ�NULL�³�������NULL�ж������ڲ����Ƿ�һ�£�һ��ֱ�ӷ��أ���һ���ٴδ���
								//һ��ֱ�ӷ��أ���һ��������ǰȻ���ٴ���
		w_, h_,			//������
		(AVPixelFormat)28,		//�������ظ�ʽBGRA
		w_, h_,   //������
		AV_PIX_FMT_YUV420P,         //������ظ�ʽ
		SWS_BILINEAR,			//ѡ��֧�ֱ任���㷨��˫���Բ�ֵ
		NULL, NULL, NULL                 //����������
	);
	if (!rgb2yuv_)
	{
		cerr << "sws_getCachedContext failed" << endl;
		return;
	}

}

void XScreen_get_task::Main()
{
	auto device = screen_.get_device();
	auto sur = screen_.get_sur();
	if (!device || !sur) return;
	while (!is_exit_)
	{
		//4 ץ��
		device->GetFrontBufferData(0, sur);

		//5 ȡ������
		D3DLOCKED_RECT rect;
		ZeroMemory(&rect, sizeof(rect));
		if ((sur->LockRect(&rect, 0, 0)) != S_OK)
		{
			cerr << "get sur error" << endl;
		}
		memcpy(buf_, rect.pBits, size_);

		//�ͷ���
		sur->UnlockRect();

		long long vpts = NowMs();
		cout << ".";
		//��ʼ����
		uint8_t* data[AV_NUM_DATA_POINTERS] = { 0 };
		int line[AV_NUM_DATA_POINTERS] = { 0 };

		data[0] = (uint8_t*)buf_;
		line[0] = w_ * 4;

		//����frame
		auto frame = av_frame_alloc();
		frame->height = h_;
		frame->width = w_;
		frame->format = AV_PIX_FMT_YUV420P;
		auto re = av_frame_get_buffer(frame, 0);
		frame->linesize[0] = w_;
		frame->linesize[1] = w_ / 2;
		frame->linesize[2] = w_ / 2;

		re = sws_scale(rgb2yuv_,
			data,        //��������
			line,
			0,
			h_,
			frame->data,
			frame->linesize
		);
		if (re < 0) return;
		cout << "T" << flush;
		//frame->pts = vpts- offset_;

		//Next(frame);

		unique_lock<mutex> lock(mux_);
		if (!next_ && frame->buf[0])
		{
			av_frame_unref(frame_);
			frame_ = frame;
		}
		else
		{
			auto pd = new PData(frame, vpts, offset_);
			Next(pd);
			av_frame_unref(frame);
		}

		this_thread::sleep_for(1ms);
	}
	//������Դ
	screen_.close();
	delete []buf_;
	buf_ = nullptr;
	size_ = 0;

	//if (frame_)
	//{
	//	av_frame_free(&frame_);
	//	frame_ = nullptr;
	//}
	sws_freeContext(rgb2yuv_);
	rgb2yuv_ = nullptr;

	w_ = -1;
	h_ = -1;

	offset_ = 0;
	return;
}



AVFrame* XScreen_get_task::GetFrame()
{
	unique_lock<mutex> lock(mux_);
	if (!frame_ || !frame_->buf[0]) return nullptr;
	auto f = av_frame_alloc();
	f->height = frame_->height;
	f->width = frame_->width;
	auto re = av_frame_ref(f, frame_);   //���ü�1
	if (re != 0)
	{
		av_frame_free(&f);
		LOGERROR("XDecodeTask::GetFrame() ERROR");
		return nullptr;
	}
	return f;

}