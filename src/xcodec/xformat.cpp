 #include "xformat.h"
#include "xtools.h"
using namespace std;
extern "C" {
#include<libavformat/avformat.h>
}
//Ԥ����ָ����
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")

static int TimeoutCallback(void* para)
{
	auto xf = (XFormat*)para;
	if (xf->IsTimeout()) return 1;  //��ʱ�˳�read
	//cout << "TimeoutCallback" << endl;
	return 0; //��������
}

void XFormat::set_c(AVFormatContext* c)
{
	unique_lock<mutex> lock(mux_);
	if (c_)  //����ԭֵ
	{
		if (c_->oformat)  //���������
		{
			if (c_->pb)
				avio_closep(&c_->pb);
			avformat_free_context(c_);
			
		}
		else if (c_->iformat)  //����������
		{
			avformat_close_input(&c_); 
		}
	}

	c_ = c;
	if (!c_)
	{
		is_conneted_ = false;
		return;
	}
	is_conneted_ = true;
	//��ʱ ���ڳ�ʱ�ж�
	last_time_ = NowMs();
	//�趨��ʱ����ص�
	if (time_out_ms_ > 0)
	{
		AVIOInterruptCB cb = { TimeoutCallback, this };
		c_->interrupt_callback = cb;
	}


	//���������Ƿ�����Ƶ����Ƶ��
	audio_index_ = -1;
	video_index_ = -1;
	for (int i = 0; i < c->nb_streams; i++)
	{
		if (c->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio_index_ = i;
			audio_time_base_.den = c->streams[i]->time_base.den;
			audio_time_base_.num = c->streams[i]->time_base.num;
		}
		else if (c->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_index_ = i;
			video_time_base_.den = c->streams[i]->time_base.den;
			video_time_base_.num = c->streams[i]->time_base.num;
			video_codec_id_ = c->streams[i]->codecpar->codec_id;
		}
	}
}


bool XFormat::CopyPara(int stream_index, AVCodecContext* dst)
{
	unique_lock<mutex>lock(mux_);
	if (!c_)
	{
		return false;
	}
	if (stream_index<0 || stream_index>c_->nb_streams)
	{
		return false;
	}
	auto re = avcodec_parameters_to_context(dst, c_->streams[stream_index]->codecpar);
	if (re < 0)
	{ 
		return false;
	}
	return true;
}


/// <summary>
/// ���Ʋ��� �̰߳�ȫ
/// </summary>
/// <param name="stream_index">��Ӧc->streams�±�</param>
/// <param name="dst">�������</param>
/// <returns>�Ƿ�ɹ�</returns>
bool XFormat::CopyPara(int stream_index, AVCodecParameters* dst)
{
	unique_lock<mutex>lock(mux_);
	if (!c_)
	{
		return false;
	}
	if (stream_index<0 || stream_index>c_->nb_streams)
	{ 
		return false;
	}
	auto re = avcodec_parameters_copy(dst, c_->streams[stream_index]->codecpar);
	if (re < 0)
	{
		return false;
	}
	return true;
}


bool XFormat::RescaleTime(AVPacket* pkt, long long offset_pts, XRational time_base)
{
	AVRational in_time_base;
	in_time_base.num = time_base.num;
	in_time_base.den = time_base.num;

	return RescaleTime(pkt, offset_pts, &in_time_base) ;
}

bool XFormat::RescaleTime(AVPacket* pkt, long long offset_pts, AVRational* time_base)
{
	if (!pkt || !time_base) return false;
	unique_lock<mutex>lock(mux_);

	if (!c_)
	{
		return false;
	}
	auto out_stream = c_->streams[pkt->stream_index];

	pkt->pts = av_rescale_q_rnd(pkt->pts - offset_pts, *time_base,
		out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

	pkt->dts = av_rescale_q_rnd(pkt->dts - offset_pts, *time_base,
		out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

	pkt->duration = av_rescale_q(pkt->duration, *time_base, out_stream->time_base);

	pkt->pos = -1;

	return true;
}

long long XFormat::RescaleToMs(long long pts, int index)
{
	unique_lock<mutex>lock(mux_);
	if (!c_ || index<0 ||index>c_->nb_streams)
	{
		return 0;
	}
	auto in_timebase = c_->streams[index]->time_base;
	AVRational out_timebase = { 1, 1000 };  //���timebase ����
	return av_rescale_q(pts , in_timebase, out_timebase);
}


void XFormat::set_time_out_ms(int ms)
{
	unique_lock<mutex>lock(mux_);
	this->time_out_ms_ = ms; 
	//���ûص�����������ʱ�˳�
	if (c_)
	{
		AVIOInterruptCB cb = { TimeoutCallback, this };
		c_->interrupt_callback  = cb;
	}
}



std::shared_ptr<XPara> XFormat::CopyVideoPara()
{
	int index = video_index();
	shared_ptr<XPara> re;
	unique_lock<mutex>lock(mux_);
	if (index < 0 || !c_) return re;
	re.reset(XPara::Create());  //������ָ�븳ֵ
	*re->time_base =  c_->streams[index]->time_base;
	int f = avcodec_parameters_copy(re->para, c_->streams[index]->codecpar);
	
	//��ȡ������ʱ��
	if ((c_->streams[index]->duration) > 0)
		re->total_ms = av_rescale_q(c_->streams[index]->duration, c_->streams[index]->time_base, { 1, 1000 });
	
	//����������ʽ FLV
	else
		re->total_ms = av_rescale_q(c_->duration, c_->streams[index]->time_base, { 1,1 });
	if (f < 0)
	{
		LOGERROR("avcodec_parameters_copy");
	}

	return re;
}


std::shared_ptr<XPara> XFormat::CopyaudioPara()
{
	int index = audio_index();
	shared_ptr<XPara> re;
	unique_lock<mutex>lock(mux_);
	if (index < 0 || !c_) return re;
	re.reset(XPara::Create());  //������ָ�븳ֵ
	*re->time_base = c_->streams[index]->time_base;
	int f = avcodec_parameters_copy(re->para, c_->streams[index]->codecpar);
	//��ȡ������ʱ��
	if ((c_->streams[index]->duration) > 0)
		re->total_ms = av_rescale_q(c_->streams[index]->duration, c_->streams[index]->time_base, { 1, 1000 });
	else
		re->total_ms = av_rescale_q(c_->duration, c_->streams[index]->time_base, {1,1});
	if (f < 0)
	{
		LOGERROR("avcodec_parameters_copy");
	}

	return re;
}