#pragma once
#include "xtools.h"
#include "xencode.h"

class XEncode;
class SwsContext;

class XCODEC_API XEncodeTask :
    public XThread
{
public:
    //编码的长宽和编码器类型
    bool Open(int w, int h, int codec_id = -1);

    //打开音频
    bool Open(int codec_id = -1);

    bool is_open() { return is_open_; }

    //责任链处理函数
    void Do(AVFrame* f) override;

    //责任链处理函数
    void Do(PData* d) override;

    AVCodecParameters* GetvideoPara() { return enc_.CopyVideoPara(); }

    void Set_index(bool is_video) { is_video_ = is_video; }

    void Stop();

    void set_stream_index(int index) { stream_index_ = index; }

    AVRational* get_time_base() { return enc_time_base_; }

protected:
    XEncode enc_;
    void Main() override;
    bool is_open_ = false;
    bool is_video_ = true;
    std::mutex mux_;
    int width_ = 0;
    int height_ = 0;
    int stream_index_ = 0;
    AVRational* enc_time_base_;
    XAVFrameList frames_;
    PDataList datas;
};

