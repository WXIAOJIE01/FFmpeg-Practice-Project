#include "xplayer.h"

extern "C" {
#include<libavcodec/avcodec.h>
}

bool XPlayer::Open(const char* url, void* winid)
{
	//���װ
	if (!demux_.Open(url))
		return false;
	//��Ƶ����
	auto vp = demux_.CopyVideoPara();
	if (vp)
	{
		this->total_ms_ = vp->total_ms;
		hw_device_ = winid;
		if (!video_decode_.Open(vp->para))
		{
			return false;
		}
		//���ڹ�����Ƶ��
		video_decode_.set_stream_index(demux_.video_index());

		//�����������ޣ���Ȼpkt_list����ж�������
		video_decode_.set_block_size(100);

		//frame����
		//video_decode_.set_frame_cache(true);
		//��Ƶ��Ⱦ
		if (!view_)
			view_ = XVideoView::Create();
		view_->set_win_id(winid);
		if (!view_->Init(vp->para))  
			return false;
	}

	auto ap = demux_.CopyAudioPara();
	if (ap)
	{
		if (!audio_decode_.Open(ap->para))
		{
			return false;
		}
		//���ڹ�����Ƶ����
		audio_decode_.set_stream_index(demux_.audio_index());

		audio_decode_.set_block_size(100);

		//frame����
		audio_decode_.set_frame_cache(true);

		//��ʼ����Ƶ����
		 XAudioPlay::Instance()->Open(&(*ap));

	}
	else
	{
		demux_.set_syn_type(XSYN_VIDEO);		//������Ƶͬ��
	}

	//���װ���ݴ�����ǰ�� 
	demux_.set_next(this);
	return true;
}

void XPlayer::Do(AVPacket* pkt)
{
	if (audio_decode_.is_open())
	{
		audio_decode_.Do(pkt);
	}
	if (video_decode_.is_open())
	{
		video_decode_.Do(pkt); 
	}

}


void XPlayer::Start()
{
	demux_.Start();
	if (audio_decode_.is_open())
	{
		audio_decode_.Start();
	}
	if (video_decode_.is_open())
	{
		video_decode_.Start();
	}
	 
	XThread::Start();  //����������Ƶͬ�����߳�
}

//��Ⱦ��Ƶ ������Ƶ
void XPlayer::Update()
{
	//��Ⱦ��Ƶ
	if (view_)
	{
		auto f = video_decode_.GetFrame();
		if (f)
		{
			if(video_decode_.Get_hw_nocopy())
				video_decode_.DrawFrame(f, video_decode_.CopyCodecContext(), hw_device_);
			else
				view_->DrawFrame(f);
			XFreeFrame(&f);
		} 
	}
	//��Ƶ����
	auto au = XAudioPlay::Instance();
	auto f = audio_decode_.GetFrame();
	if (!f) return;
	au->Push(f);
	XFreeFrame(&f);
}


void XPlayer::Main()
{
	long long syn = 0;
	auto au = XAudioPlay::Instance();
	auto ap = demux_.CopyAudioPara();
	auto vp = demux_.CopyVideoPara();
	//û����Ƶ��ֱ���˳�������Ҫ����ͬ��
	video_decode_.set_time_base(vp->time_base);
	while (!is_exit_) 
	{ 
		if (is_pause())
		{
			MSleep(1);
			continue; 
		}
		this->pos_ms_ = video_decode_.cur_ms();
		if (ap)
		{
			syn = XRescale(au->cur_pts(), ap->time_base, vp->time_base);
			audio_decode_.set_syn_pts(au->cur_pts());  //˵��Ϊ�˻��壬����ʵ�����û��ʲô����
			video_decode_.set_syn_pts(syn);
			MSleep(1); 
		}

	}
}

void XPlayer::Stop()
{
	Exit();
	demux_.Exit();
	audio_decode_.Exit();
	video_decode_.Exit();

	XThread::Stop();
	demux_.Stop();
	audio_decode_.Stop();
	video_decode_.Stop();
	
	Wait();
	demux_.Wait();
	audio_decode_.Wait();
	video_decode_.Wait();
	if (view_)
	{
		view_->Close();
		delete view_;
		view_ = nullptr;
	}
	XAudioPlay::Instance()->Close();

}

void XPlayer::SetSpeed(float s)
{
	XAudioPlay::Instance()->SetSpeed(s);
}

//������Ƶ����λ�� ms
bool XPlayer::Seek(long long ms)
{
	demux_.Seek(ms);

	long long seek_pts = demux_.get_seek_pts();


	long long v_t = 0;
	auto ap = demux_.CopyAudioPara();
	auto vp = demux_.CopyVideoPara();
	v_t = XRescale(seek_pts, vp->time_base, ap->time_base);

	//audio_decode_.set_seek_pos(v_t);

	audio_decode_.Clear();
	video_decode_.Clear();

	while (!is_exit_)
	{
		AVPacket* pkt = demux_.Get_read();
		if (!pkt) break;
		if (pkt->stream_index == demux_.audio_index())
		{
			av_packet_free(&pkt);
			continue;
			if (pkt->pts < v_t)
			{
				av_packet_free(&pkt);
				continue;
			}
			//д�뻺��
			demux_.Next(pkt);
			av_packet_unref(pkt);
			continue;
		}

		//��Ƶ����
		video_decode_.SendPacket(pkt);
		AVFrame* f = video_decode_.Get_seekFrame();
		if (!f)
		{
			continue;
		}
		if (f->pts >= seek_pts)
		{
			XFreeFrame(&f);
			break;
		}
		XFreeFrame(&f);
	}

	video_decode_.set_seek_pos(seek_pts);

	
	return true;
}

void XPlayer::Pause(bool is_pause)
{
	XThread::Pause(is_pause);
	demux_.Pause(is_pause);
	audio_decode_.Pause(is_pause);
	video_decode_.Pause(is_pause);
	XAudioPlay::Instance()->Pause(is_pause);
}

void XPlayer::SetVolum(int v)
{
	if (v > 128|| v < 0) v = 64;
	XAudioPlay::Instance()->SetVolum(v);
}
