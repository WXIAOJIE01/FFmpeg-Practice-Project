#include "xdecode_task.h"
#include<iostream>
#include <windows.h>
#include <d3d9.h>
using namespace std;
extern "C" {
#include<libavformat/avformat.h>
#include<libavcodec/avcodec.h>
}

struct DXVA2DevicePriv {
	HMODULE d3dlib;
	HMODULE dxva2lib;
	HANDLE device_handle;
	IDirect3D9* d3d9;
	IDirect3DDevice9* d3d9device;
};

void XDecodeTask::set_time_base(AVRational* time_base)
{
	if (!time_base) return; 
	unique_lock<mutex> lock(mux_);
	if (time_base_)
		delete time_base_;
	time_base_ = new AVRational();
	time_base_->den = time_base->den;
	time_base_->num = time_base->num;
}

bool XDecodeTask::SendPacket(AVPacket* pkt)
{
	unique_lock<mutex> lock(mux_);
	pkt_list_.Push(pkt);
	auto pkt_ = pkt_list_.Pop();
	if (!pkt_) return false;
	if (!decode_.Send(pkt_))
	{
		return false;
	}
	av_packet_free(&pkt_);
	av_packet_free(&pkt);
	return true;
}

AVFrame* XDecodeTask::Get_seekFrame()
{
	unique_lock<mutex> lock(mux_);
	AVFrame* f = av_frame_alloc();
	decode_.Recv(f);
	return f;
}


void XDecodeTask::Clear()
{
	pkt_list_.Clear();
	unique_lock<mutex> lock(mux_);
	//����frames
	while (!frames_.empty())
	{
		av_frame_free(&(frames_.front()));
		frames_.pop_front();
	} 
	cur_pts_ = -1;  //��ǰ���뵽��pts(�Խ�������Ϊ׼ )
	decode_.Clear();
}

void XDecodeTask::Stop()
{
	XThread::Stop();
	pkt_list_.Clear();
	unique_lock<mutex> lock(mux_);
	decode_.set_c(nullptr);
	is_open_ = false;

	if (time_base_)
		delete time_base_;
	time_base_ = nullptr; 

	//����frames
	while (!frames_.empty())
	{
		av_frame_free(&(frames_.front()));
		frames_.pop_front();
	}

}

/// <summary>
/// �򿪽�����
/// </summary>
/// <param name="para"></param>
/// <returns></returns>
bool XDecodeTask::Open(AVCodecParameters* para)
{

	if (!para) {
		LOGERROR("para is null");
		return false;  
	}
	unique_lock<mutex> lock(mux_);
	is_open_ = false;  //��֤һ���̰߳�ȫ
	auto c = decode_.Create(para->codec_id, false);
	if (!c)
	{
		LOGERROR("decode_.create failed!");
		return false;
	}
	//������Ƶ����
	int re = avcodec_parameters_to_context(c, para);
	if (re < 0)
	{
		LOGERROR("avcodec_parameters_to_context error");
	}
	decode_.set_c(c);

	//�����Ƿ�֧��Ӳ������
	if (is_hw_&&para->width>0)
	{
		if (!decode_.InitHW())
		{
			return false;
		}
		if (!is_copy_)
		{
			decode_.SetCopy(false);
		}
	}

	if (!decode_.Open())
	{
		LOGERROR("decode_.open failed!");
		return false;
	}
	LOGINFO("decode open success");

	is_open_ = true;
	return true;
}

//������������
void XDecodeTask::Do(AVPacket* pkt)
{
	cout << "#" << flush;
	if (!pkt || pkt->stream_index != stream_index_)  //�ж��Ƿ�����Ƶ
	{
		return;
	}
	pkt_list_.Push(pkt);
	if (block_size_ <= 0) return;
	while (!is_exit_)
	{
		if (pkt_list_.Size() > block_size_)
		{
			MSleep(1);
			continue; 
		}
		break;
	}
}

//�߳�������
void XDecodeTask::Main()
{
	{
		unique_lock<mutex> lock(mux_);
			if (!frame_)
				frame_ = av_frame_alloc();
	}

	while (!is_exit_)
	{
		if (is_pause())//��ͣ
		{
			MSleep(1);
			continue;
		}

		//ͬ��
		while (!is_exit_)
		{
			//if (frame_cache_) break;
			if (syn_pts_ >= 0 && cur_pts_ > syn_pts_)
			{
				MSleep(1);
				continue;
			}
			break;
		}

		auto pkt = pkt_list_.Pop();
		if (!pkt)
		{
			this_thread::sleep_for(1ms);
			continue; 
		}
		//���͵������߳�
		bool re = decode_.Send(pkt);
		av_packet_free(&pkt);
		if (!re)
		{
			this_thread::sleep_for(1ms);
			continue;
		} 
		{
			unique_lock<mutex> lock(mux_);
			if (decode_.Recv(frame_))
			{
				cout << "@" << flush;
				if (frame_->pts < seek_pts_)
				{
					need_view_ = false;
					cur_pts_ = seek_pts_;
				}
				else
				{
					need_view_ = true;
					cur_pts_ = frame_->pts;
				}
				//ת���ɺ���
				if(time_base_)
					cur_ms_ = av_rescale_q(frame_->pts, *time_base_, { 1, 1000 });
				
			}
			//��Ƶ���֣���Ҫ��ǰ�������������ܶ�֡����ȻӰ��ܴ� 
			if (frame_cache_)
			{
				if (frame_->pts >= seek_pts_)
				{
					need_round = true;
					cur_pts_ = frame_->pts;
					auto f = av_frame_alloc();
					av_frame_ref(f, frame_);   //���ü�����1;
					frames_.push_back(f);
				}
				else {
					//cur_pts_ = seek_pts_;
					//syn_pts_ = cur_pts_ - 1000;
					//need_round = false;
				}

			}
		}
		

		this_thread::sleep_for(1ms);
	}
	{
		unique_lock<mutex> lock(mux_);
		if (frame_)
			av_frame_free(&frame_);
	}

}

AVFrame* XDecodeTask::GetFrame()
{
	unique_lock<mutex> lock(mux_);
	if (frame_cache_)
	{
		if (frames_.empty()|| !need_round) return nullptr;
		auto f = frames_.front();
		frames_.pop_front();
		return f;
	}
	if (!need_view_ || !frame_ || !frame_->buf[0]) return nullptr;
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
	need_view_ = false; 
	return f;
}


void XDecodeTask::DrawFrame(AVFrame* frame, AVCodecContext* c, void* hwnd1)
{
	if (!frame->data[3] || !c) return;
	HWND hwnd = (HWND)hwnd1;
	auto surface = (IDirect3DSurface9*)frame->data[3];
	auto ctx = (AVHWDeviceContext*)c->hw_device_ctx->data;
	auto priv = (DXVA2DevicePriv*)ctx->user_opaque;
	auto device = priv->d3d9device;

	static RECT viewport;
	viewport.left = 0;
	viewport.right = frame->width;
	viewport.top = 0;
	viewport.bottom = frame->height;

	// ������ʾ���ھ��
	device->Present(&viewport, &viewport, hwnd, 0);

	// ��̨�������
	static IDirect3DSurface9* back = nullptr;
	if (!back)
		device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back);
	device->StretchRect(surface, 0, back, &viewport, D3DTEXF_LINEAR);
}