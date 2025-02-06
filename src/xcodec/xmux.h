#pragma once
#include "xformat.h"
#include "xtools.h"
/// <summary>
///媒体封装
/// </summary>
class XCODEC_API XMux :
    public XFormat
{
public:
    /// <summary>
    /// 打开封装
    /// </summary>
    /// <param name="url"></param>
    /// <returns></returns>
    static AVFormatContext* Open(const char* url,
        AVCodecParameters* video_para = nullptr,
        AVCodecParameters* audio_para = nullptr,
        bool is_rtmp = false
    );


    //写入头部信息
    bool writeHead();

    bool Write(AVPacket* pkt);
    bool WriteEnd(); 

    //音视频时间基础
    void set_src_video_time_base(AVRational* tb);
    void set_src_audio_time_base(AVRational* tb);
     
    ~XMux();
private:
    AVRational* src_video_time_base_ = nullptr;
    AVRational* src_audio_time_base_ = nullptr;

    long long begin_video_pts = -1;  //原视频开始时间
    long long begin_audio_pts = -1;  //原音频开始时间

};

