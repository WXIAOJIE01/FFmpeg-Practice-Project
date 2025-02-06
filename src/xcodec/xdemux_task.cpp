#include "xdemux_task.h"
using namespace std;
extern "C" {
#include<libavformat/avformat.h>
}

bool XDemuxTask::Seek(long long ms)
{
	auto vp = demux_.CopyVideoPara();
	if (!vp) return false;
	auto pts = av_rescale_q(ms, { 1, 1000 }, *vp->time_base);
	seek_pts_ = pts;
	return  demux_.Seek(pts, video_index());
}

void XDemuxTask::Stop()
{
	XThread::Stop();
	demux_.set_c(nullptr);
}

AVPacket* XDemuxTask::Get_read()
{
	AVPacket* pkt = av_packet_alloc();
	if (!demux_.Read(pkt))
	{
		return nullptr;
	}
	return pkt;
}

void XDemuxTask::Main()
{
	AVPacket pkt;
	while (!is_exit_)
	{
		if (is_pause())
		{
			MSleep(1);
			continue;
		} 
		if (!(demux_.Read(&pkt)))
		{
			//读取失败
			cout << "-" << flush;
			if (! demux_.is_conneted())
			{
				Open(url_, timeout_ms_);
			}
			this_thread::sleep_for(chrono::milliseconds(1));
			continue; 
		}
		cout << "." << flush;



		if (syn_type_ == XSYN_VIDEO &&
			pkt.stream_index == demux_.video_index())
		{
			auto dur = demux_.RescaleToMs(pkt.duration, pkt.stream_index);
			if (dur <= 0) dur = 40;
			MSleep(dur);
		}
		////为了锁定seek的帧
		//if (pkt.stream_index == demux_.video_index() && pkt.pts < seek_pts_)
		//{
		//	av_packet_unref(&pkt);
		//	continue;
		//}
		Next(&pkt);
		av_packet_unref(&pkt);
		this_thread::sleep_for(std::chrono::milliseconds(1));
		 
	}
}



bool XDemuxTask::Open(std::string url, int timueout_ms)
{
	LOGDEBUG("XDemuxTask::Open begin");
	demux_.set_c(nullptr);
	this->url_ = url;
	this->timeout_ms_ = timueout_ms;
	auto c = demux_.Open(url.c_str());
	if (!c) return false;
	demux_.set_c(c);
	demux_.set_time_out_ms(timueout_ms);
	LOGDEBUG("XDemuxTask::Open end ");
	return true; 
}