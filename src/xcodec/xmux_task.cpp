#include "xmux_task.h"
using namespace std;
extern "C" {
#include<libavformat/avformat.h>
}


bool XMuxTask::Open(const char* url,
    AVCodecParameters* video_para,
    AVRational* video_time_base,
    AVCodecParameters* audio_para,
    AVRational* audio_time_base
)
{
    auto c = xmux_.Open(url, video_para, audio_para);
    if (!c) return false;
    xmux_.set_c(c);
    xmux_.set_src_video_time_base(video_time_base);
    xmux_.set_src_audio_time_base(audio_time_base);
    return true;
}

void XMuxTask::Do(AVPacket* pkt)
{
    if (!pkt) return;
    pkts_.Push(pkt);
    Next(pkt);
}

void XMuxTask::Main()
{
    xmux_.writeHead();

    //�ҵ��ؼ�֡
    while (!is_exit_)
    {
        unique_lock<mutex> lock(mux_);
        auto pkt = pkts_.Pop();
        if (!pkt)
        {
            MSleep(1);
            continue;
        }
        if (pkt->stream_index == xmux_.video_index()
            && pkt->flags & AV_PKT_FLAG_KEY) //�ؼ�֡
        {
            xmux_.Write(pkt);
            av_packet_free(&pkt);
            break;
        }
        //��������Ƶ�ؼ�֡
        av_packet_free(&pkt); 
    }
    while (!is_exit_)
    {
        unique_lock<mutex> lock(mux_);
        auto pkt = pkts_.Pop();
        if (!pkt)
        {
            MSleep(1);
            continue;
        }

        xmux_.Write(pkt);
        av_packet_free(&pkt);
    }
    xmux_.WriteEnd();
    pkts_.Clear();
    xmux_.set_c(nullptr);

}
